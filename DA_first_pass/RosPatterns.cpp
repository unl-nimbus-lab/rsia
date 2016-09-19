/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/*
 * @author: Nishant Sharma
 */


#include <clang/Analysis/Analyses/RosPatterns.h>
#include <clang/Analysis/Analyses/Dominators.h>
#include <clang/AST/PrettyPrinter.h>
#include <fstream>
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/AST/Mangle.h>
#include <clang/Analysis/Analyses/Dominators.h>
#include <clang/AST/Expr.h>
#include <clang/AST/StmtVisitor.h>
#include <yaml-cpp/yaml.h>
#include <clang/Analysis/Analyses/CFGReachabilityAnalysis.h>

using namespace clang;
using namespace std;

#define DEBUG ;
// llvm::errs() << __LINE__ << "\n";

namespace {
	/*
	 * \class
	 *
	 * \brief Main class for the implementation of detection and dumping of the detected ROS patterns
	 */
	class RosPatternsImpl {
	public:

		//for comparing strings
		struct ltstr
		{
			bool operator()(std::string s1, std::string s2) const
			{
				return (s1.compare(s2) < 0);
			}
		};

		/*
		 * Member Variables
		 * non static variables are used to hold values just at the function level
		 */
		AnalysisDeclContext &analysisContext;
		//data structures to hold pattern information
		///to hold the mapping from variable name to rosparam name
		static std::map<std::string, std::string, ltstr> variableToRosparam;

		///to hold the mapping from variable name to published topic
		static std::map<std::string, std::string, ltstr> variableToPublisher;
		std::map<std::string, std::string, ltstr> nsvariableToPublisher;

		///to hold the mapping from variable to subscribed topic
		//static std::map<std::string, std::string, ltstr> variableToSubscriber;
		std::map<std::string, std::string, ltstr> nsVariableToSubscriber;

		///to hold the mapping from subscribed topic to attached callback function
		//static std::map<std::string, std::string, ltstr> subscriberToCallback;
		std::map<std::string, std::string, ltstr> nsSubscriberToCallback;

		///to hold the list of functions that are timer callbacks
		//static std::vector<std::string> timerToCallback;
		std::vector<std::string> nsTimerToCallback;


		/**
		 * types of loop based patterns
		 */
		enum SleepPattern{
			RATE_WHILE_SLEEP,
			WHILE_SLEEP,
			NONE
		};

		/*
		 * Next three blocks hold information for the RATE_WHILE_SLEEP and WHILE_SLEEP patterns
		 */
		map<string, const CFGBlock*> rateBlocks;
		vector<const CFGBlock*> whileBlocks;
		vector<const CFGBlock*> sleepBlocks;
		map<const CFGBlock*, SleepPattern> blockPatterns;
		map< const VarDecl *, bool> variableStorage;


		const Stmt* findTopic(const Stmt *s);

		/*
		 * Member functions
		 */

		///checks the Stmt for the timer callback and add it to the timer callback list
		void resolveTimer(const Stmt *stmt, ItaniumMangleContext *mangler);


		///checkes whether the passed argument of the function call is a string litral of not
		///returns bool true if it's a string, in any case returns the argument as a string
		bool isArgAString(const CXXMemberCallExpr *expr, int argNum, string &value);

		//returns the left most variable name for different stmt types
		string getVariableName(const CXXOperatorCallExpr *cxxOperatorCallExpr);
		string getVariableName(const BinaryOperator *binaryOperator);
		string getVariableName(const DeclStmt *declStmt);
		string getVariableName(const Stmt *stmt);

		///returns the string version of the SLEEP PATTERNS
		string getSleepPatternString(SleepPattern &sleepPattern);

		///go through the cfg to detect the loop based patterns
		void resolveWhileSleepLoop(CFG *cfg,
								   ItaniumMangleContext *mangler,
								   ReachableConditions *reachableConditions,
								   DominatorTree &dom,
								   SleepPattern &sleepPattern);

		///returns whether the function was a CXXMemberCallExpr or not, takes calleeName and calleeNameString as reference
		///calleeName is returned with the mangled name of the function
		///calleeNameString is returned with the original name of the function
		bool getFunctionCallName(const Stmt *S, ItaniumMangleContext *mangler, string &calleeName, string &calleeNameString);
		string mangleName(ItaniumMangleContext * mangler, const NamedDecl *function);

		/**
		 * Detect patterns like publisher, subscriber and many more
		 */
		void detectPatterns(CFG *cfg, ItaniumMangleContext * mangler, AnalysisDeclContext &analysisContext);

		/*
		 * These functions dump required information in the required functions
		 */
		void writePublishers();
		void writeSubscribers();
		void writeTimers();
		void dumpPatterns(CFG *cfg,
						  ItaniumMangleContext * mangler,
						  ReachableConditions *reachableConditions,
						  DominatorTree &dom,
						  AnalysisDeclContext &analysisContext,
						  SleepPattern &sleepPattern);

		/*
		 * Constructor
		 */
		RosPatternsImpl(AnalysisDeclContext &ac)
				: analysisContext(ac),
				  policy(langOpts),
				  langOpts()
		{
			langOpts.CPlusPlus = true;
			policy = PrintingPolicy(langOpts);
		}
	private:
		//pretty print option globally set at class level
		PrintingPolicy policy;
		LangOptions langOpts;
	};

	/*
	 * Outside class definition for static variables of the class
	 */
	std::map<std::string, std::string, RosPatternsImpl::ltstr> RosPatternsImpl::variableToRosparam;
	std::map<std::string, std::string, RosPatternsImpl::ltstr> RosPatternsImpl::variableToPublisher;
/*
 * Removed following maps from static space
 */
//	std::map<std::string, std::string, RosPatternsImpl::ltstr> RosPatternsImpl::variableToSubscriber;
//	std::vector<std::string> RosPatternsImpl::timerToCallback;
//	std::map<std::string, std::string, RosPatternsImpl::ltstr> RosPatternsImpl::subscriberToCallback;

}

/*
 * To easily capture ROS::RATE statement using the AST instead of the CFG
 */
namespace
{
	class TransferFunctions : public StmtVisitor<TransferFunctions>
	{
		RosPatternsImpl &rp;
		const CFGBlock *currentBlock;
	public:
		TransferFunctions(RosPatternsImpl &im,
						  const CFGBlock *CurrentBlock)
				: rp(im), currentBlock(CurrentBlock) {
			rp.rateBlocks.clear();

		}

		//void VisitIfStmt(IfStmt *IS);
		void VisitDeclStmt(DeclStmt *DS);

	};
}

/*
 * Detects ros::rate and push the block to the rate blocks
 */
void TransferFunctions::VisitDeclStmt(DeclStmt *DS) {
	for (DeclStmt::decl_iterator DI=DS->decl_begin(), DE = DS->decl_end();
		 DI != DE; ++DI){
		if (VarDecl *VD = dyn_cast<VarDecl>(*DI))
		{
			if(VD->hasInit())
			{
				std::string s = QualType::getAsString(VD->getInit()->getType().split());
				if(s.compare("ros::Rate") == 0)
				{
					/* *
					 * add any ros::rate to the rate blocks
					 * TODO: get the variable name for ros::rate as well to check for exactly rate.sleep rather than just sleep otherwise might result in false positives
					 */
					rp.rateBlocks[VD->getNameAsString()] = (currentBlock);
					//cout << "Ros::Rate vd name: " << VD->getNameAsString() << endl;
				}
			}
		}
	}
}

/*
 * TODO: warning unsed, in future try removing this function and executing to decide whether to keep or delete the function
 */
static RosPatternsImpl &getImpl(void *x) {
	return *((RosPatternsImpl *) x);
}

/*
 * This function setup's name mangling, dominator analysis, reachable conditions, and almost anything else
 * required later to detect and dump ros patterns. For someone looking first time at this code, this is the
 * entry point of the analysis.
 * TODO: In near future we would want to make it interprocedural (currently intraprocedural)
 */
RosPatterns* RosPatterns::computeRosPatterns(AnalysisDeclContext &analysisContext) {

	// make sure it's a function otherwise exit
	if(!isa<FunctionDecl>(*(analysisContext.getDecl())))
		return NULL;

	// No CFG?  Bail out.
	CFG *cfg = analysisContext.getCFG();
	if (!cfg)
		return nullptr;

	// The analysis currently has scalability issues for very large CFGs.
	// Bail out if it looks too large.
	if (cfg->getNumBlockIDs() > 300000)
		return nullptr;
	//cerr << __LINE__  << "\n";
	//create instance of our analyzer
	RosPatternsImpl *RC = new RosPatternsImpl(analysisContext);
	//cerr << __LINE__  << "\n";
	//create name mangler, we are creating the mangler of type Itanium
	/*
	 * FROM this point onwards function names will be mangled unless specefied otherwise
	 */
	clang::DiagnosticOptions diagnosticOptions;
	clang::TextDiagnosticPrinter *pTextDiagnosticPrinter =  new clang::TextDiagnosticPrinter(llvm::outs(), &diagnosticOptions);
	llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
	clang::DiagnosticsEngine *pDiagnosticsEngine = new clang::DiagnosticsEngine(pDiagIDs,
																&diagnosticOptions,
																pTextDiagnosticPrinter);
	ItaniumMangleContext * mangler = ItaniumMangleContext::create(analysisContext.getASTContext(), *pDiagnosticsEngine);
	//cerr << __LINE__  << "\n";
	//create instance of reachable conditions analysis
	ReachableConditions* reachableConditions = ReachableConditions::create(analysisContext);
	//cerr << __LINE__  << "\n";
	//get dominater analysis
	DominatorTree dom;
	/*
	 * Next line is commented because dom was failing for ROS Nodes
	 */
	//dom.buildDominatorTree(analysisContext);
	//cerr << __LINE__  << "\n";
	//read and store node related info
	//RC->resolveRosNode();
	//read and store ros parameter list
	//RC->readRosParams();
	//cerr << __LINE__  << "\n";
	//detect if we have a loop sleep based pattern
	RosPatternsImpl::SleepPattern sleepPattern = RosPatternsImpl::NONE;
	RC->resolveWhileSleepLoop(cfg,mangler, reachableConditions, dom, sleepPattern);
	//cerr << __LINE__  << "\n";
	//detect if we have any other patterns
	RC->detectPatterns(cfg, mangler, analysisContext);
	//cerr << __LINE__  << "\n";
	//write the rest of the patterns
	RC->writePublishers();
	//cout << __LINE__ << endl;
	RC->writeSubscribers();
	//cout << __LINE__ << endl;
	RC->writeTimers();

	/*
	 * dump function call list with loop-sleep pattern info
	 * TODO: this will change when the pattern becomes interprocedural
	 */
	RC->dumpPatterns(cfg, mangler, reachableConditions, dom, analysisContext, sleepPattern);
	//cerr << __LINE__  << "\n";
	//cerr << __LINE__  << "\n";
	return new RosPatterns(RC);
}


void RosPatternsImpl::dumpPatterns(CFG *cfg, ItaniumMangleContext *mangler, ReachableConditions *reachableConditions,
								   DominatorTree &dom, AnalysisDeclContext &analysisContext,SleepPattern &sleepPattern) {
	//get function name
	const FunctionDecl *cmd = cast<FunctionDecl>(analysisContext.getDecl());
	string functionName = mangleName(mangler, cmd);
	//cerr << functionName.c_str()  << "\n";
	ofstream patternDump("model.log", ofstream::app);

	//TODO:output to yaml files rather than this
	//print the mangled name for the function, otherwise infinite loops might occur
	patternDump << "FUNCTION_START\n";
	patternDump << functionName << "$DEPENDENT\n";
	patternDump << "CALL_LIST_START\n";

	//for all CFG blocks
	for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it) {
		const CFGBlock *block = *it;
		//get ros::rate information

		TransferFunctions TF(*this, block);

		/*
		 * Assign sleep pattern label
		 */

		RosPatternsImpl::SleepPattern blockPattern = RosPatternsImpl::NONE;
		for (int i = 0; i < sleepBlocks.size(); ++i) {
			if(sleepBlocks[i] == block)
				blockPattern = sleepPattern;
		}

		//for all CFG elements
		for (CFGBlock::const_iterator cbit = block->begin(), cbei = block->end(); cbit != cbei; ++cbit)
		{
			const CFGElement &elem = *cbit;

			//if it's not a statement type
			if (elem.getKind() != clang::CFGElement::Kind::Statement)
				continue;

			//then retrieve the stmt
			const Stmt *S = elem.castAs<CFGStmt>().getStmt();

			//only dump for function calls
			if(!isa<CallExpr>(*S))
				continue;

			//retrive called function's name
			string calleeName = "UNKNOWN";
			string calleeNameString = "UNKNOWN";
			bool isCXXMemberCallExpr = getFunctionCallName(S, mangler, calleeName, calleeNameString);

			//TODO: We need to change dump type
			//skip publish calls as they will go later in the dump
			//TODO: replace these checks with their mangled names
			//TODO: verify tf details for both static and regular tf
			//FIXME: DUMPING publish calls as normal calls as wells as publish calls
		/*	if(isCXXMemberCallExpr && (!calleeNameString.compare("publish") || !calleeNameString.compare("sendTransform") || !calleeNameString.compare("unlockAndPublish"))){
				continue;
			}
			else */{
				bool conditions = reachableConditions->isReachableConditionsPresent(S);
				if(blockPattern != NONE) { //sleep pattern can either be RATE_WHILE_SLEEP or WHILE_SLEEP
					if(!conditions) {
						patternDump << calleeName << "$INDEPENDENT$" << getSleepPatternString(blockPattern) << "$" << "\n";
					}
					else{
						patternDump << calleeName << "$GREEN_MAYBE_DEPENDENT$" << getSleepPatternString(blockPattern) << "$" << "CONDITION" << "\n";
//						patternDump << calleeName << "$GREEN_MAYBE_DEPENDENT$" << getSleepPatternString(sleepPattern) << "$" << conditions << "\n";
					}
				}
				else {
					patternDump << calleeName << "$DEPENDENT$NONE$" << conditions << "\n";
				}
			}
		}
	}

	patternDump << "CALL_LIST_END\n";

	/*
	 * do a publish side dump now
	 */

	patternDump << "PUBLISH_LIST_START\n";

	//for all CFG blocks
	for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it) {
		const CFGBlock *block = *it;
		//get ros::rate information

		TransferFunctions TF(*this, block);


		RosPatternsImpl::SleepPattern blockPattern = RosPatternsImpl::NONE;
		for (int i = 0; i < sleepBlocks.size(); ++i) {
			if(sleepBlocks[i] == block)
				blockPattern = sleepPattern;
		}

		//for all CFG elements
		for (CFGBlock::const_iterator cbit = block->begin(), cbei = block->end(); cbit != cbei; ++cbit)
		{
			const CFGElement &elem = *cbit;

			//if it's not a statement type
			if (elem.getKind() != clang::CFGElement::Kind::Statement)
				continue;

			//then retrieve the stmt
			const Stmt *S = elem.castAs<CFGStmt>().getStmt();

			//only dump for function calls
			if(!isa<CallExpr>(*S))
				continue;

			//retrive called function's name
			string calleeName = "UNKNOWN";
			string calleeNameString = "UNKNOWN";
			bool isCXXMemberCallExpr = getFunctionCallName(S, mangler, calleeName, calleeNameString);

			//TODO: We need to change dump type
			//skip publish calls as they will go later in the dump
			//TODO: replace these checks with their mangled names
			//TODO: verify tf details for both static and regular tf
			if(isCXXMemberCallExpr && (!calleeNameString.compare("publish") || !calleeNameString.compare("sendTransform") || !calleeNameString.compare("unlockAndPublish"))){
				bool conditions = reachableConditions->isReachableConditionsPresent(S);

				//make sure the type is publisher or tf or realtimepublisher
				const CXXMemberCallExpr *mce = cast<CXXMemberCallExpr>(S);
				const MemberExpr *me = dyn_cast<MemberExpr>(mce->getCallee());
				Expr *base = me->getBase();
				if((base->getType().getAsString().find("Publisher") == std::string::npos) &&
				   	(base->getType().getAsString().find("TransformBroadcaster") == std::string::npos) &&
				   	(base->getType().getAsString().find("RealtimePublisher") == std::string::npos))
					continue;

				/*
				 * retrive the corresponding variable name now
				 */
				std::string publisher;
				if(isa<DeclRefExpr>(*(base->IgnoreParenImpCasts())))
					publisher =	(cast<DeclRefExpr>(*(base->IgnoreParenImpCasts()))).getDecl()->getDeclName().getAsString();
				if(isa<MemberExpr>(*(base->IgnoreParenImpCasts())))
					publisher = (cast<MemberExpr>(*(base->IgnoreParenImpCasts()))).getMemberDecl()->getDeclName().getAsString();

				if(blockPattern != NONE) {
					if(!conditions) {
						patternDump << publisher << "$" << this->variableToPublisher[publisher] << "$INDEPENDENT$" << getSleepPatternString(blockPattern) << "$" << "\n";
					}
					else{
						patternDump << publisher << "$" << this->variableToPublisher[publisher] << "$GREEN_MAYBE_DEPENDENT$" << getSleepPatternString(blockPattern) << "$" << "CONDITIONS" << "\n";
					}
				}
				else {
					if(!conditions) {
						patternDump << publisher << "$" << this->variableToPublisher[publisher] << "$DEPENDENT$NONE$" << "\n";
					}
					else{
						patternDump << publisher << "$" << this->variableToPublisher[publisher] << "$DEPENDENT$NONE$" << "CONDITIONS" << "\n";
					}
				}
			}
			else {
				continue;
			}
		}
	}
	patternDump << "PUBLISH_LIST_END\n";
	patternDump << "FUNCTION_END\n";
	patternDump.close();
}

/*
 * For now we don't care about sleep block leaks : ask Nishant for more
 */
void RosPatternsImpl::resolveWhileSleepLoop(CFG *cfg, ItaniumMangleContext *mangler, ReachableConditions *reachableConditions,
											DominatorTree &dom, SleepPattern &sleepPattern) {
	bool isSleepFound = false;
	//for all CFG blocks assign default value to the blocks
	for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it) {
		blockPatterns[*it] = RosPatternsImpl::NONE;
	}

	//for all CFG blocks
	for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it) {
		const CFGBlock *block = *it;

		//get blocks which have ros::rate in them
		TransferFunctions TF(*this, block);

		/*
		 * get
		 */
		// Apply the transfer function for all decl Stmts in the block.
		for (CFGBlock::const_iterator it = block->begin(),
					 ei = block->end(); it != ei; ++it) {
			const CFGElement &elem = *it;

			if (!elem.getAs<CFGStmt>())
				continue;

			const Stmt *S = elem.castAs<CFGStmt>().getStmt();

			if(isa<DeclStmt>(S))
				TF.Visit(const_cast<Stmt*>(S));
		}

		bool isDetected = false;

		//get blocks which have while or do while loop in them
		if(block
		   && (block->getTerminator().getStmt())) {
			if (isa<WhileStmt>(*(block->getTerminator().getStmt()))) {
//				whileBlocks.push_back(block);
				isDetected = true;
			}

			if (isa<DoStmt>(*(block->getTerminator().getStmt()))) {
//				whileBlocks.push_back(block);
				isDetected = true;
			}

			std::set<const CFGBlock*> currentWhileSleepBlockList;
			std::deque<const CFGBlock*> whileBlockList;

			const CFGBlock * stopperBlock;

			CFGReverseBlockReachabilityAnalysis reachableAnalysis(*cfg);

			if(isDetected) {
				//add all successors to a list
				int itor = 0;
				for (CFGBlock::const_succ_iterator sit = block->succ_begin(), sei = block->succ_end(); sit != sei; ++sit) {
					if(itor == 0)
						whileBlockList.push_back(*sit);
					else if(itor == 1)
						stopperBlock = *sit;
				}
				vector<const CFGBlock *> visited;
				//cerr << __LINE__  << "\n";
				//for all successors of while and it's subtree's block
				while (!whileBlockList.empty()) {
					if (whileBlockList.front() == block)
					{
						//cerr << __LINE__  << "\n";
						whileBlockList.pop_front();
						continue;
					}
					if(!whileBlockList.front())
					{
						//cerr << __LINE__  << "\n";
						whileBlockList.pop_front();
						continue;
					}
					if (!reachableAnalysis.isReachable(whileBlockList.front(), block))
					{
						//cerr << __LINE__  << "\n";
						whileBlockList.pop_front();
						continue;
					}


					//cerr << __LINE__  << "\n";
					if (const CFGBlock *current = whileBlockList.front()) {
						bool isVisited = false;
						for (unsigned int i = 0; i < visited.size(); ++i) {
							if(current == visited[i])
							{
								////cerr << __LINE__  << "\n";
								isVisited = true;
								break;
							}
						}
						if(isVisited)
						{
							//cerr << __LINE__  << "\n";
							whileBlockList.pop_front();
							continue;
						}
						//cerr << __LINE__  << "\n";
						visited.push_back(current);
						//cerr << __LINE__  << "\n";
						//we break out when we find sleep
						if (!isSleepFound) {
							//detecting sleep
							for (CFGBlock::const_iterator cbit = current->begin(), cbei = current->end();
								 cbit != cbei; ++cbit) {
								const CFGElement &elem = *cbit;

								//if it's not a statement type
								if (elem.getKind() != clang::CFGElement::Kind::Statement)
									continue;

								//then retrieve the stmt
								const Stmt *S = elem.castAs<CFGStmt>().getStmt();

								//move further only if it's of interest
								if (!isa<CallExpr>(*S))
									continue;

								string calleeName, calleeNameString;

								//get the name of the function
								getFunctionCallName(S, mangler, calleeName, calleeNameString);
								if (!calleeNameString.compare("sleep")) {
									isSleepFound = true;
								}
							}
						}
						//adding successors
						for (CFGBlock::const_succ_iterator sit = current->succ_begin(), sei = current->succ_end();
							 sit != sei; ++sit) {
							if(*sit != stopperBlock)
								whileBlockList.push_back(*sit);
						}
					}
					currentWhileSleepBlockList.insert(whileBlockList.front());
					//popped current block after processing
					whileBlockList.pop_front();
				}
				//llvm::errs() << "WHILE EXIT\n";
				currentWhileSleepBlockList.insert(const_cast<CFGBlock *>(block));

				if(isSleepFound)
				{
					bool isRate = false;
					bool isWhile = true; // change back to false if doing a while check
					//evolve sleep block
					for (set<const CFGBlock*>::iterator it = currentWhileSleepBlockList.begin(); it != currentWhileSleepBlockList.end(); ++it) {
						//if while dominates the current block
						//removed dom based checked as dom was failing, also not checking for while as we only search for sleep after we have found while
						/*for (unsigned int i = 0; i < whileBlocks.size(); ++i) {
							if (dom.dominates(whileBlocks[i], block)) {
								isWhile = true;
								break;
							}
						}*/

						//if rate dominates the current node
						//removed dom based checked as dom was failing
						/*for (map<string, const CFGBlock *>::iterator i = rateBlocks.begin(); i != rateBlocks.end(); ++i) {
							if (dom.dominates(i->second, block)) {
								isRate = true;
								break;
							}
						}*/
						//TODO: change this basic check
						if(!rateBlocks.empty())
							isRate = true;
						sleepBlocks.push_back(*it);
					}

					if(isWhile)
					{
						sleepPattern = WHILE_SLEEP;
						if(isRate)
						{
							sleepPattern = RATE_WHILE_SLEEP;
						}
					}
					else{
						sleepPattern = NONE;
					}
				}
				else{
					sleepPattern = NONE;
				}
			}
		}
	}
}

string RosPatternsImpl::getSleepPatternString(SleepPattern &sleepPattern) {
	if(sleepPattern == RATE_WHILE_SLEEP)
		return "RATE_WHILE_SLEEP";
	else if(sleepPattern == WHILE_SLEEP)
		return "WHILE_SLEEP";
	else
		return "NONE";
}

string RosPatternsImpl::mangleName(ItaniumMangleContext *mangler, const NamedDecl *function) {
	//call proper mangler based on it's type
	std::string TypeString;
	llvm::raw_string_ostream ss(TypeString);
	/*
	 * 		if - A Constructor
	 * else if - A Destructor
	 * else	   - regular function name
	 */
	if(isa<CXXConstructorDecl>(function))
	{
		const CXXConstructorDecl *cxxCD = cast<CXXConstructorDecl>(function);
		//complete oonstructor if not abstract
		if(!cxxCD->getParent()->isAbstract())
			mangler->mangleCXXCtor(cxxCD, clang::CXXCtorType::Ctor_Complete, ss);
		else
			mangler->mangleCXXCtor(cxxCD, clang::CXXCtorType::Ctor_Base, ss);
	}
	else if(isa<CXXDestructorDecl>(function))
	{
		const CXXDestructorDecl *cxxDD = cast<CXXDestructorDecl>(function);
		//complete oonstructor if not abstract
		if(!cxxDD->isVirtual())
			mangler->mangleCXXDtor(cxxDD, clang::CXXDtorType::Dtor_Deleting, ss);
		else
			mangler->mangleCXXDtor(cxxDD, clang::CXXDtorType::Dtor_Complete, ss);
	}
	else if(isa<NamedDecl>(*function))
	{
		mangler->mangleName(function, ss);
	}
	return ss.str();
}

bool RosPatternsImpl::getFunctionCallName(const Stmt *S, ItaniumMangleContext *mangler, string &calleeName, string &calleeNameString) {
	bool isCXXMemberCallExpr = false;
	//cout << __LINE__ << endl;
	if(isa<CXXMemberCallExpr>(*S)){
		isCXXMemberCallExpr = true;
		const CXXMemberCallExpr *mce = cast<CXXMemberCallExpr>(S);
		const FunctionDecl *functionDecl = mce->getDirectCallee();
		//cout << __LINE__ << endl;
		if(functionDecl == NULL)
			return true;
		//cout << __LINE__ << endl;

		const MemberExpr *me = dyn_cast<MemberExpr>(mce->getCallee());
		if(me == NULL)
			return true;
		//cout << __LINE__ << endl;

		calleeNameString = me->getMemberNameInfo().getAsString();
		if(functionDecl)
			calleeName = this->mangleName(mangler, functionDecl);
	}
	//cout << __LINE__ << endl;
	//call should not be either of these two
		//fixme: have CXXoperator as a seperate mangler
	else if (!(isa<CXXOperatorCallExpr>(*S)) && !(isa<CUDAKernelCallExpr>(*S))){
		const CallExpr *ce = cast<CallExpr>(S);
		//cout << __LINE__ << endl;

		if(ce->getDirectCallee() == NULL)
			return false;
		//cout << __LINE__ << endl;

		calleeNameString = ce->getDirectCallee()->getNameAsString();
		if(ce->getDirectCallee() != NULL)
			calleeName = mangleName(mangler, ce->getDirectCallee());
	}
	//cout << __LINE__ << endl;
	return isCXXMemberCallExpr;
}

/*
 * TODO: resolve remaps
 */
void RosPatternsImpl::detectPatterns(CFG *cfg, ItaniumMangleContext *mangler, AnalysisDeclContext &analysisContext) {
	/*
	 * We need to detect the right pattern and call it's resolver if required
	 * Patterns: Publisher, subscriber , realtime publisher, tf send , tf recieve, timer.
	 */
	for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it)
	{
		string topic;
		bool foundPublisher = false;
		bool foundSubscriber = false;

		for (CFGBlock::const_iterator cbit = (*it)->begin(), cbei = (*it)->end(); cbit != cbei; ++cbit)
		{

			const CFGElement &elem = *cbit;


			if(elem.getKind() == clang::CFGElement::Kind::Initializer && (foundPublisher)){

				const CXXCtorInitializer *cxxCtorInitializer = elem.castAs<CFGInitializer>().getInitializer();
				//cout << __LINE__ << " " << cxxCtorInitializer->getMember()->getNameAsString() << endl;
				string variable = cxxCtorInitializer->getMember()->getNameAsString();
				if(variable.empty()) continue;
				variableToPublisher[variable] = topic;
				nsvariableToPublisher[variable] = topic;
				//cout << "publisher" << variable << "  " <<topic << endl;
				foundPublisher = false;
				continue;
			}
			if(elem.getKind() == clang::CFGElement::Kind::Initializer && (!foundPublisher) && (!foundSubscriber)){
				DEBUG
				const CXXCtorInitializer *cxxCtorInitializer = elem.castAs<CFGInitializer>().getInitializer();
				if(!cxxCtorInitializer->getMember()) continue;
				//cout << __LINE__ << " " << cxxCtorInitializer->getMember()->getNameAsString() << endl;
				DEBUG
				std::string s = QualType::getAsString(cxxCtorInitializer->getMember()->getType().split());
				DEBUG
				//cout << s << endl;
				DEBUG
				//cout << "kala sha kala \n" << cxxCtorInitializer->getMember()->getNameAsString() << " \n  ";
				// cxxCtorInitializer->getInit()->printPretty(llvm::errs(),0,policy); llvm::errs()<<"\n";
				if(s.find("realtime_tools::RealtimePublisher") != string::npos) {
					DEBUG
				/*	string variable = cxxCtorInitializer->getMember()->getNameAsString();
					if(variable.empty()) continue;
					string variable = cxxCtorInitializer->getMember()->getNameAsString();
					if(variable.empty()) continue;

					cout << "publisher" << variable << "  " <<topic << endl;
					foundPublisher = false;
					continue;*/
				}
			}

			//if it's not a statement type
			if (elem.getKind() != clang::CFGElement::Kind::Statement)
				continue;

			//then retrieve the stmt
			const Stmt *S = elem.castAs<CFGStmt>().getStmt();

			//S->printPretty(llvm::errs(),0,policy); llvm::errs() << "\n";
			//cout << S->getStmtClassName() << endl;
			/* detect cxxcontruct expr */

			if(isa<CXXNewExpr>(S) && !foundPublisher && !foundSubscriber){

				const CXXConstructExpr * cxxConstructExpr = (cast<CXXNewExpr>(S))->getConstructExpr();
				if(!cxxConstructExpr) continue;
				//S->printPretty(llvm::errs(),0,policy); llvm::errs() << "\n";
				string typeString;
				llvm::raw_string_ostream ss(typeString);
				std::string s = QualType::getAsString(cxxConstructExpr->getType().split());
				DEBUG
				//cout << s << endl;
				DEBUG
				if(s.find("realtime_tools::RealtimePublisher") != string::npos) {
					cxxConstructExpr->getArg(1)->printPretty(ss, 0, policy);
					//cout << ss.str() << endl;
					if (ss.str()[0] == '\"' || ss.str()[0] == '"') {
						topic = "";
						for (int i = 1; i < ss.str().length() - 1; ++i) {
							topic += ss.str()[i];
							//	cout << topic << endl;
						}
					}
					//cout << "REALTIME PUB CONST << " << topic << endl;
					if(!topic.empty()){
						DEBUG
						foundPublisher = true;
						continue;
					}
				}
			}

			if(isa<CXXConstructExpr>(S) && !foundPublisher && !foundSubscriber){
				const CXXConstructExpr * cxxConstructExpr = cast<CXXConstructExpr>(S);
				//S->printPretty(llvm::errs(),0,policy); llvm::errs() << "\n";
				string typeString;
				llvm::raw_string_ostream ss(typeString);
				std::string s = QualType::getAsString(cxxConstructExpr->getType().split());
				DEBUG
				//cout << s << endl;
				DEBUG
				if(s.find("realtime_tools::RealtimePublisher") != string::npos) {
					cxxConstructExpr->getArg(1)->printPretty(ss, 0, policy);
					//cout << ss.str() << endl;
					if (ss.str()[0] == '\"' || ss.str()[0] == '"') {
						topic = "";
						for (int i = 1; i < ss.str().length() - 1; ++i) {
							topic += ss.str()[i];
							//	cout << topic << endl;
						}
					}
					//cout << "REALTIME PUB CONST << " << topic << endl;
					if(!topic.empty()){
						DEBUG
						foundPublisher = true;
						continue;
					}
				}
			}


			//move further only if it's of interest
			if(!isa<CXXMemberCallExpr>(*S)
			   && !isa<CXXOperatorCallExpr>(*S)
			   && !isa<DeclStmt>(*S)
			   && !isa<BinaryOperator>(*S))
				continue;

			if(isa<DeclStmt>(S) && !foundPublisher && !foundSubscriber){
				const DeclStmt *declStmt = cast<DeclStmt>(S);
//				DEBUG
				for (auto it = declStmt->decl_begin(); it != declStmt->decl_end(); it++) {
					if(const VarDecl * varDecl = cast<VarDecl>(*it)){
//						DEBUG
						//declStmt->printPretty(llvm::errs(),0,policy); llvm::errs() << "\n";
						string variable = varDecl->getNameAsString();
						if(varDecl->hasInit())
						{
//							DEBUG
							std::string topic = "";
							std::string s = QualType::getAsString(varDecl->getInit()->getType().split());
							//cout << s << endl;
							if(s.find("message_filters::Subscriber") != string::npos) {
//								DEBUG
								string typeString;
								llvm::raw_string_ostream ss(typeString);
								const Expr *initExpr = varDecl->getInit();
								//cout << initExpr->getStmtClassName() << endl;
								//if(isa<ExprWithCleanups>(initExpr)){
								//	initExpr = (cast<ExprWithCleanups>(initExpr))->getSubExpr();
								//}
								initExpr = initExpr->IgnoreParenImpCasts();
								initExpr = initExpr->IgnoreImplicit();
								initExpr = initExpr->IgnoreImpCasts();
								initExpr = initExpr->IgnoreCasts();
								const CXXConstructExpr* cxxConstructExpr;
								if (isa<CXXConstructExpr>(initExpr) && (cxxConstructExpr = cast<CXXConstructExpr>(initExpr))){
									cxxConstructExpr->getArg(1)->printPretty(ss, 0, policy);
//									cout << ss.str()[0] << endl;
									if(ss.str()[0] == '\"' || ss.str()[0] == '"') {
										topic = "";
										for (int i = 1; i < ss.str().length() - 1; ++i) {
											topic += ss.str()[i];
										//	cout << topic << endl;
										}
									}
								}
								//cout << "MessageFilterSubscriber vd name: " << varDecl->getNameAsString() << endl;
								nsVariableToSubscriber[variable] = topic;
								nsSubscriberToCallback[topic] = "unknown";
							}
							//for real time publisher
							if(s.find("realtime_tools::RealtimePublisher") != string::npos) {
//								DEBUG
								string typeString;
								llvm::raw_string_ostream ss(typeString);
								const Expr *initExpr = varDecl->getInit();
								//cout << initExpr->getStmtClassName() << endl;
								initExpr = initExpr->IgnoreParenImpCasts();
								initExpr = initExpr->IgnoreImplicit();
								initExpr = initExpr->IgnoreImpCasts();
								initExpr = initExpr->IgnoreCasts();
								//if(isa<ExprWithCleanups>(initExpr)){
								//	initExpr = (cast<ExprWithCleanups>(initExpr))->getSubExpr();
								//}
								const CXXConstructExpr* cxxConstructExpr;
								if (isa<CXXConstructExpr>(initExpr) && (cxxConstructExpr = cast<CXXConstructExpr>(initExpr))){
									cxxConstructExpr->getArg(1)->printPretty(ss, 0, policy);
									//llvm::errs() << ss.str() << "\n";
									if(ss.str()[0] == '\"' || ss.str()[0] == '"') {
										topic = "";
										for (int i = 1; i < ss.str().length() - 1; ++i) {
											topic += ss.str()[i];
										}
									}
								}
								//cout << "RealTime Pub name: " << varDecl->getNameAsString() << endl;
								if(!topic.empty())
								{
									variableToPublisher[variable] = topic;
									nsvariableToPublisher[variable] = topic;
								}
							}
						}
					}
				}
			}




			//checking if it's a timer or a walltimer
			if(isa<CXXMemberCallExpr>(*S) && !foundPublisher && !foundSubscriber) {
				string calleeName, calleeNameString;
				getFunctionCallName(S,mangler,calleeName,calleeNameString);

				//if it's a timer
				if(!calleeNameString.compare("createTimer") ||
						!calleeNameString.compare("createWallTimer") )
				{
					resolveTimer(S, mangler);
				}

				//if it's a publish call
				/*
				 * For a publish call we only want to store the variable name and it's topic - topic right now and for variable look at else part
				 */
				if(!calleeNameString.compare("advertise")){
					//cout << "publisher detected " << __LINE__ << endl;
					const CXXMemberCallExpr *mce = dyn_cast<CXXMemberCallExpr>(S); //we expect the stmt to be a CXXMember call expr at this point
					topic = "";
					bool isString = isArgAString(mce,0/*publish topic is first argument*/,topic);
					if(!isString)
					{
						//cout << __LINE__ << endl;
						if(topic.empty())
						{
							//cout << __LINE__ << endl;
							continue;
						}
						//resolveTopicVariable(topic);
						if(topic.empty())
						{
							//cout << __LINE__ << endl;
							continue;
						}
					}
					//cout << "publisher detected " << topic << endl;
					foundPublisher = true;
				}

				if(!calleeNameString.compare("advertiseServer")){
					nsVariableToSubscriber["unknown"] = "unknown";
					nsSubscriberToCallback["unknown"] = "unknown";
					continue;
				}

				//if it's a subcriber call
				if(!calleeNameString.compare("subscribe")){
					//cout << "subscriber detected" << endl;
					const CXXMemberCallExpr *mce = dyn_cast<CXXMemberCallExpr>(S); //we expect the stmt to be a CXXMember call expr at this point
					topic = "";
					bool isString = isArgAString(mce,0/*subscribe topic is first argument*/,topic);
					//cout << __LINE__ << " " << topic << endl;
					if(!isString)
					{
						//cout << __LINE__ << endl;
						if(topic.empty())
						{
							//cout << __LINE__ << endl;
							nsVariableToSubscriber["unknown"] = "unknown";
							nsSubscriberToCallback["unknown"] = "unknown";
							continue;
						}
						//resolveTopicVariable(topic);
					}
					string callback;
					//get callback now
					DEBUG
					string mangledCallback, originalCallback;
					int noParams = mce->getDirectCallee()->getNumParams();
					if(noParams < 3){
						//cout << __LINE__ << endl;
						nsVariableToSubscriber["unknown"] = "unknown";
						nsSubscriberToCallback["unknown"] = "unknown";
						continue;
					}
					const Expr * callbackExpr = mce->getArg(2);
					//cout<< callbackExpr->getStmtClassName() << "\n";
					callbackExpr = callbackExpr->IgnoreParenImpCasts();
					callbackExpr = callbackExpr->IgnoreImplicit();
					callbackExpr = callbackExpr->IgnoreImpCasts();
					callbackExpr = callbackExpr->IgnoreCasts();
					/*
					 * handle constructor here as well boost::bind
					 */
					if(isa<UnaryOperator>(callbackExpr))
					{
						const DeclRefExpr * dre = (cast<DeclRefExpr>((cast<UnaryOperator>(callbackExpr)->getSubExpr())));
						mangledCallback = mangleName(mangler,dre->getFoundDecl());
						//cout << __LINE__ << " " << topic << " " << mangledCallback << endl;
						nsSubscriberToCallback[topic] = mangledCallback;
						DEBUG
						foundSubscriber = true;
						continue;
					}
						/*regular cases*/
					else if (isa<DeclRefExpr>(callbackExpr)){
						DEBUG
						const DeclRefExpr * dre = (cast<DeclRefExpr>((callbackExpr)));
						mangledCallback = mangleName(mangler,dre->getFoundDecl());
						//cout << __LINE__ << " " << topic << " " << mangledCallback << endl;
						nsSubscriberToCallback[topic] = mangledCallback;
						DEBUG
						foundSubscriber = true;

					}
					else
					{
						DEBUG
						nsVariableToSubscriber["unknown"] = "unknown";
						nsSubscriberToCallback["unknown"] = "unknown";
						continue;

					}
				}

				/*
				 * Next is tf subscribe utility - then publish
				 */
				if(!calleeNameString.compare("lookupTransform") || !calleeNameString.compare("waitForTransform")){

					DEBUG DEBUG

					nsVariableToSubscriber["tempTF"] = "tf";
					nsSubscriberToCallback["tf"] = "temp-tf";
				}

				if(!calleeNameString.compare("sendTransform")) {
					DEBUG
					const CXXMemberCallExpr *cme = cast<CXXMemberCallExpr>(S);
					const Expr *member = cme->getImplicitObjectArgument();
					DEBUG
					if(isa<MemberExpr>(member))
					{
						DEBUG
						const MemberExpr * memberE = cast<MemberExpr>(member);
						DEBUG
						//llvm::errs() << memberE->getStmtClassName() << "\n";
						DEBUG
//						if (const DeclRefExpr *dre = cast<DeclRefExpr >(memberE->getBase())) {
							string typeDecl = memberE->getType().getAsString();
							if (typeDecl.find("StaticTransformBroadcaster")!=string::npos){
								variableToPublisher[memberE->getMemberNameInfo().getAsString()] = "static_tf";
								nsvariableToPublisher[memberE->getMemberNameInfo().getAsString()] = "static_tf";
							}
							else{
								variableToPublisher[memberE->getMemberNameInfo().getAsString()] = "tf";
								nsvariableToPublisher[memberE->getMemberNameInfo().getAsString()] = "tf";
							}
//						}
					}

						//check for the type of the member and also save the variable name
					else {
						variableToPublisher["tempTF"] = "tf";
						nsvariableToPublisher["tempTF"] = "tf";
					}
				}

			}
			/*
			 * now to detect the variables
			 */
				/*
				 * TODO: add cxxmembercallexpr
				 */
			else{
				if(foundPublisher)
				{
					if(topic.empty())
					{
						//cout << "publisher" << __LINE__ << endl;
						foundPublisher = false;
						continue;
					}
					//cout << "publisher " << __LINE__ << endl;
					string variable = getVariableName(S);
					if(variable.empty()) continue;
					variableToPublisher[variable] = topic;
					nsvariableToPublisher[variable] = topic;
					//cout << "publisher " << variable << "  " <<topic << endl;
					foundPublisher = false;
				}
				if(foundSubscriber)
				{
					//cout << __LINE__ << endl;
					string variable = getVariableName(S);
					if(variable.empty()) continue;
					nsVariableToSubscriber[variable] = topic;
					//cout << __LINE__ << " "<< variable << " " << topic << endl;
					foundSubscriber = false;
				}
			}
		}
	}
}


/*
 * Down casts the type to call the right function
 */
string RosPatternsImpl::getVariableName(const Stmt* stmt) {
	//cout << __LINE__ << endl;

	//cout << "CXXSTMT " << stmt->getStmtClassName() << " " ;
	//stmt->printPretty(llvm::errs(),0,policy);
	//cout << endl;

	if(isa<CXXOperatorCallExpr>(stmt))
	{
		const CXXOperatorCallExpr *coce = cast<CXXOperatorCallExpr>(stmt);
		//cout << __LINE__ << endl;

		return getVariableName(coce);
	}
	if(isa<BinaryOperator>(stmt))
	{
		const BinaryOperator *bo = cast<BinaryOperator>(stmt);
		//cout << __LINE__ << endl;

		return getVariableName(bo);
	}
	if(isa<DeclStmt>(stmt))
	{
		const DeclStmt *ds = cast<DeclStmt>(stmt);
		//cout << __LINE__ << endl;

		return getVariableName(ds);
	}
	/*if(isa<CXXCtorInitializer>(stmt))
	{
		cout << "CXXConstructExpr" << endl;

		const CXXCtorInitializer * cxxCtorInitializer = cast<CXXCtorInitializer>(stmt);
		cout << cxxCtorInitializer->getMember()->getNameAsString() << endl;

	}*/
	/*
	 * handle constroctor case here
	 */
//	if(isa<CXXMemberCallExpr>(stmt))
//	{
//		const CXXMemberCallExpr *mce = cast<CXXMemberCallExpr>(stmt);
//		const FunctionDecl *functionDecl = mce->getDirectCallee();
//		const MemberExpr *me = dyn_cast<MemberExpr>(mce->getCallee());
//		cout <<"This-> " << me->getMemberNameInfo().getAsString() << endl;
//	}
	return "";
}

string RosPatternsImpl::getVariableName(const CXXOperatorCallExpr *cxxOperatorCallExpr){
	/*
	 * Only return variable name if it's a publisher or a subscriber
	 */
	if(isa<MemberExpr>(cxxOperatorCallExpr->getArg(0)))
	{
		const Expr * expr = cxxOperatorCallExpr->getArg(0);
		const MemberExpr *ME = cast<MemberExpr>(expr);
		// check if it's either publisher or subscriber
		if((ME->getType().getAsString().find("Publisher") == std::string::npos) &&
				(ME->getType().getAsString().find("Subscriber") == std::string::npos) )
			return "";

			return ME->getMemberDecl()->getDeclName().getAsString();
	}
	else if( isa<DeclRefExpr>(cxxOperatorCallExpr->getArg(0)) )
	{
		DeclRefExpr *DRE = cast<DeclRefExpr>(const_cast<Expr *>(cxxOperatorCallExpr->getArg(0)));
		if((DRE->getType().getAsString().find("Publisher") == std::string::npos) &&
				(DRE->getType().getAsString().find("Subscriber") == std::string::npos))
			return "";
		return DRE->getDecl()->getDeclName().getAsString();
	}
//	if(isa<ArraySubscriptExpr>(cxxOperatorCallExpr->getCallee()))
//	if(const ArraySubscriptExpr * ase = cast<ArraySubscriptExpr>(cxxOperatorCallExpr->getCallee())){
//		cout << __LINE__ << endl;
//		if(const DeclRefExpr *dre = cast<DeclRefExpr> (ase->getBase())){
//			cout << __LINE__ << endl;
//			return dre->getDecl()->getName();;
//		}
//		if(const MemberExpr * me = cast<MemberExpr> (ase->getBase()))
//		{
//			cout << __LINE__ << endl;
//			return me->getMemberDecl()->getName();
////		}
//	}
	return "";
}

string RosPatternsImpl::getVariableName(const BinaryOperator *binaryOperator){
	if(binaryOperator->getOpcode() != clang::BO_Assign)
		return "";

	Expr * left = binaryOperator->getLHS()->IgnoreParenImpCasts();
	/*
	 * Only return variable name if it's a publisher or a subscriber
	 */
	/*if((left->getType().getAsString().find("Publisher") == std::string::npos) &&
	   (left->getType().getAsString().find("Subscriber") == std::string::npos))
		return "";*/
	DEBUG
	if(isa<ArraySubscriptExpr>(left))
	if(const ArraySubscriptExpr * ase = cast<ArraySubscriptExpr>(left)){
		//cout << __LINE__ << endl;
		if(const DeclRefExpr *dre = cast<DeclRefExpr> (ase->getBase())){
			//cout << __LINE__ << endl;
			return dre->getDecl()->getName();;
		}
		if(const MemberExpr * me = cast<MemberExpr> (ase->getBase()))
		{
			//cout << __LINE__ << endl;
			return me->getMemberDecl()->getName();
		}
	}
DEBUG
	if(isa<MemberExpr>(left))
	if(const MemberExpr * me = cast<MemberExpr> (left))
	{
		//cout << __LINE__ << endl;
		return me->getMemberDecl()->getName();
	} 
DEBUG
	if(!isa<DeclRefExpr> (*left))
		return "";
	DeclRefExpr *dre = cast<DeclRefExpr> (left);

	return dre->getDecl()->getName();
}

string RosPatternsImpl::getVariableName(const DeclStmt *declStmt){
	const Decl *d = declStmt->getSingleDecl();
	const ValueDecl * vd = cast<ValueDecl>(d);
	return vd->getNameAsString();
}

bool RosPatternsImpl::isArgAString(const CXXMemberCallExpr *expr, int argNum, string &value){
	// Added for null string check
	if(expr->getDirectCallee() == NULL)
	{
		    //cout << "topic " << __LINE__ << endl;
		value = "";
		return false;
	}
	string typeString;
	llvm::raw_string_ostream ss(typeString);
	std::string paramType = expr->getDirectCallee()->getParamDecl(argNum)->getType().getAsString();
	if(paramType.find("string") != std::string::npos ||
			(paramType.find("char") != std::string::npos))
	{
		expr->getArg(argNum)->printPretty(ss,0,policy);
		   // cout << "topic " << __LINE__ << ss.str() << endl;
		    value = ss.str();
		if(ss.str()[0] == '\"' || ss.str()[0] == '"') {
				value ="";
			for (int i = 1; i < ss.str().length() - 1; ++i) {
				value += ss.str()[i];
			}
		}
		return true;
	}
	else
	{
	//	cout << "topic " << __LINE__ << endl;
		expr->getArg(argNum)->printPretty(ss,0,policy);
		value = ss.str();
		return false;
	}
}

const Stmt* RosPatternsImpl::findTopic(const Stmt *s){
	for (Stmt::const_child_iterator chit = s->child_begin(), chei = s->child_end(); chit != chei; ++chit) {
		const Stmt * child = *chit;
		if(isa<const StringLiteral>(*child))
			return child;
		else
			return findTopic(child);
	}
	return NULL;
}

void RosPatternsImpl::resolveTimer(const Stmt *stmt, ItaniumMangleContext *mangler) {
	const CXXMemberCallExpr *mce = dyn_cast<CXXMemberCallExpr>(stmt); //we expect the stmt to be a CXXMember call expr at this point
	const Expr *callback = mce->getArg(1); //first argument is the name of the function
	if(!isa<UnaryOperator>(callback))
		return;
	const DeclRefExpr * dre = (cast<DeclRefExpr>((cast<UnaryOperator>(callback)->getSubExpr())));
	string mangledCallback = mangleName(mangler,dre->getFoundDecl());

//	string typeString;
//	llvm::raw_string_ostream ss(typeString);
//	callback->printPretty(ss,0,policy);
	this->nsTimerToCallback.push_back(mangledCallback);
}

/*
 * For Timer we only write the callback function name
 */
void RosPatternsImpl::writeTimers() {
	std::ofstream TimerDot("timers.log", std::ios::app);
	for (std::vector<std::string>::iterator
				 it = nsTimerToCallback.begin(), ei = nsTimerToCallback.end();
		 it != ei; ++it)
	{
		TimerDot << *it << "$0" <<"\n";
	}
	TimerDot.close();
}

void RosPatternsImpl::writePublishers() {
	/*
	 * TODO: change publishers to /tmp/namesomething and output in yaml
	 */
	std::ofstream PubHandles;
	PubHandles.open("publishers.log", ios::app);
	for (std::map<std::string, std::string>::iterator
				 it = nsvariableToPublisher.begin(), ei = nsvariableToPublisher.end();
		 it != ei; ++it) {
		if(it->first.empty()) continue;
		PubHandles << it->first << "$" << it->second << "\n";
	}
	PubHandles.close();
}

void RosPatternsImpl::writeSubscribers() {
	/*
	 * TODO: change subscribers to /tmp/namesomething and output in yaml
	 */
	std::ofstream SubHandles;
	SubHandles.open("subscribers.log", ios::app);
	for (std::map<std::string, std::string>::iterator
				 it = nsVariableToSubscriber.begin(), ei = nsVariableToSubscriber.end();
		 it != ei; ++it) {
		//cout << __LINE__ << " " << it->first << endl;
		SubHandles << it->first << "$" <<  nsSubscriberToCallback[it->second] <<  "$" << it->second << "\n";
	}
	SubHandles.close();

}

const void *RosPatterns::getTag() {
	static int x;
	return &x;
}

RosPatterns::RosPatterns(void *im) : impl(im) {
}

RosPatterns::~RosPatterns() {
	delete (RosPatternsImpl*) impl;
}
