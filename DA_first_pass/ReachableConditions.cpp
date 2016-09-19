//=- LiveVariables.cpp - Live Variable Analysis for Source CFGs ----------*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements Reachable Conditions analysis for source-level CFGs.
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/ReachableConditions.h"
#include "clang/AST/Stmt.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/AnalysisContext.h"
#include "clang/AST/StmtVisitor.h"
#include "llvm/ADT/DenseMap.h"

#include <deque>
#include <algorithm>
#include <vector>
#include <clang/AST/PrettyPrinter.h>
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <iostream>

using namespace clang;

namespace {
	// INFO: This is copy-pasted from ThreadSafety.c.

	class CFGBlockSet {
		llvm::BitVector VisitedBlockIDs;
	public:
		// po_iterator requires this iterator, but the only interface needed is the
		// value_type typedef.
		struct iterator { typedef const CFGBlock *value_type; };

		CFGBlockSet() {}
		CFGBlockSet(const CFG *G) : VisitedBlockIDs(G->getNumBlockIDs(), false) {}

		/// \brief Set the bit associated with a particular CFGBlock.
		/// This is the important method for the SetType template parameter.
		std::pair<llvm::NoneType, bool> insert(const CFGBlock *Block) {
			// Note that insert() is called by po_iterator, which doesn't check to
			// make sure that Block is non-null.  Moreover, the CFGBlock iterator will
			// occasionally hand out null pointers for pruned edges, so we catch those
			// here.
			if (!Block)
				return std::make_pair(None, false); // if an edge is trivially false.
			if (VisitedBlockIDs.test(Block->getBlockID()))
				return std::make_pair(None, false);
			VisitedBlockIDs.set(Block->getBlockID());
			return std::make_pair(None, true);
		}

		/// \brief Check if the bit for a CFGBlock has been already set.
		/// This method is for tracking visited blocks in the main threadsafety
		/// loop. Block must not be null.
		bool alreadySet(const CFGBlock *Block) {
			return VisitedBlockIDs.test(Block->getBlockID());
		}
	};

/// \brief We create a helper class which we use to iterate through CFGBlocks in
/// the reverse topological order.
	class TopologicallySortedCFG {
		typedef llvm::po_iterator<const CFG*, CFGBlockSet, true>  po_iterator;

		std::vector<const CFGBlock*> Blocks;

		typedef llvm::DenseMap<const CFGBlock *, unsigned> BlockOrderTy;
		BlockOrderTy BlockOrder;


	public:
		typedef std::vector<const CFGBlock*>::iterator iterator;

		TopologicallySortedCFG(const CFG *CFGraph) {
			Blocks.reserve(CFGraph->getNumBlockIDs());
			CFGBlockSet BSet(CFGraph);

			for (po_iterator I = po_iterator::begin(CFGraph, BSet),
						 E = po_iterator::end(CFGraph, BSet); I != E; ++I) {
				BlockOrder[*I] = Blocks.size() + 1;
				Blocks.push_back(*I);
			}
		}

		iterator begin() {
			return Blocks.begin();
		}

		iterator end() {
			return Blocks.end();
		}

		bool empty() {
			return begin() == end();
		}

		struct BlockOrderCompare;
		friend struct BlockOrderCompare;

		struct BlockOrderCompare {
			const TopologicallySortedCFG &TSC;
		public:
			BlockOrderCompare(const TopologicallySortedCFG &tsc) : TSC(tsc) {}

			bool operator()(const CFGBlock *b1, const CFGBlock *b2) const {
				TopologicallySortedCFG::BlockOrderTy::const_iterator b1It = TSC.BlockOrder.find(b1);
				TopologicallySortedCFG::BlockOrderTy::const_iterator b2It = TSC.BlockOrder.find(b2);

				unsigned b1V = (b1It == TSC.BlockOrder.end()) ? 0 : b1It->second;
				unsigned b2V = (b2It == TSC.BlockOrder.end()) ? 0 : b2It->second;
				return b1V > b2V;
			}
		};

		BlockOrderCompare getComparator() const {
			return BlockOrderCompare(*this);
		}
	};

	class DataflowWorklist {
  		SmallVector<const CFGBlock *, 20> worklist;
 		llvm::BitVector enqueuedBlocks;
		TopologicallySortedCFG TSC;
	public:
  		DataflowWorklist(const CFG &cfg, AnalysisDeclContext &Ctx)
    		: enqueuedBlocks(cfg.getNumBlockIDs()),
			  TSC(&cfg) {}
  
		void enqueueBlock(const CFGBlock *block);
		void enqueuePredecessors(const CFGBlock *block);
		void enqueueSuccessors(const CFGBlock *block);

		const CFGBlock *dequeue();

		void sortWorklist();
	};

}

void DataflowWorklist::enqueueBlock(const clang::CFGBlock *block) {
  	if (block && !enqueuedBlocks[block->getBlockID()])
  	{
    	enqueuedBlocks[block->getBlockID()] = true;
    	worklist.push_back(block);
  	}
}

void DataflowWorklist::enqueuePredecessors(const clang::CFGBlock *block) {
	const unsigned OldWorklistSize = worklist.size();
	for (CFGBlock::const_pred_iterator I = block->pred_begin(),
	   	E = block->pred_end(); I != E; ++I)
	{
    	enqueueBlock(*I);
  	}
  
  	if (OldWorklistSize == 0 || OldWorklistSize == worklist.size())
    	return;

  	sortWorklist();
}

void DataflowWorklist::enqueueSuccessors(const clang::CFGBlock *block) {
	const unsigned OldWorklistSize = worklist.size();
	for (CFGBlock::const_succ_iterator I = block->succ_begin(),
				 E = block->succ_end(); I != E; ++I)
	{
		enqueueBlock(*I);
	}

	if (OldWorklistSize == 0 || OldWorklistSize == worklist.size())
		return;

	sortWorklist();
}

void DataflowWorklist::sortWorklist() {
  	std::sort(worklist.begin(), worklist.end(), TSC.getComparator());
}

const CFGBlock *DataflowWorklist::dequeue() {
	if (worklist.empty())
		return 0;
	const CFGBlock *b = worklist.back();
	worklist.pop_back();
	enqueuedBlocks[b->getBlockID()] = false;
	return b;
}

namespace {
class ReachableConditionsImpl {
public:  
	AnalysisDeclContext &analysisContext;
	llvm::ImmutableSet<const AnnotatedExpr *>::Factory DSetFact;
	std::map<const CFGBlock *, ReachableConditions::Conditions> blocksEndToConditions;
	std::map<const CFGBlock *, ReachableConditions::Conditions> blocksBeginToConditions;
	std::map<const Stmt     *, ReachableConditions::Conditions> stmtsToConditions;
	std::map<const CFGBlock *, const Expr *> blockConditions;
	std::map<const CFGBlock *, AnnotatedExpr *> blockConditionsForDefault; // if statement succ that does not have else
	std::map<const CFGBlock *, bool> negFlag;
	std::map<const CFGBlock *, AnnotatedExpr> blockAnnotExpr;

	ReachableConditions::Conditions
	merge(ReachableConditions::Conditions valsA,
		ReachableConditions::Conditions valsB);

	ReachableConditions::Conditions
			addDefaultConditionToSet(const CFGBlock *block, ReachableConditions::Conditions val);

	ReachableConditions::Conditions
			runOnBlock(const CFGBlock *block, ReachableConditions::Conditions val,
					   ReachableConditions::Observer *obs = nullptr);

	void dumpBlockConditions(const SourceManager& M);

	ReachableConditionsImpl(AnalysisDeclContext &ac)
	: analysisContext(ac),
	  DSetFact(false), // Do not canonicalize ImmutableSets by default. // This is a *major* performance win.
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
}

static ReachableConditionsImpl &getImpl(void *x) {
  return *((ReachableConditionsImpl *) x);
}

//===----------------------------------------------------------------------===//
// Operations and queries on Conditions.
//===----------------------------------------------------------------------===//

std::string ReachableConditions::Conditions::getConditions(const char delim) const {
	string condition = "";

	LangOptions lo;
	lo.CPlusPlus = true;
	PrintingPolicy policy(lo);
	std::string TypeString;
	llvm::raw_string_ostream ss(TypeString);

	for (llvm::ImmutableSet<const AnnotatedExpr *>::iterator si =
			this->annotatedExpressions.begin(),
				 se = this->annotatedExpressions.end(); si != se; ++si)
	{
		if((*si)->negation)
		{
			condition += " !(";
		}

		//pretty print the expr here
		if((*si)->expression == NULL)
			continue;
		if(dyn_cast<Stmt>((*si)->expression) == NULL)
			continue;



		(*si)->expression->printPretty(ss,0,policy);

		condition += ss.str();

		if((*si)->negation)
		{
			condition += " )";
		}
		condition += delim;
	}
	return condition;
}

std::vector<std::string> ReachableConditions::Conditions::getConditions() const {
	std::vector<std::string> conditions;

	LangOptions lo;
	lo.CPlusPlus = true;
	PrintingPolicy policy(lo);
	std::string TypeString;
	llvm::raw_string_ostream ss(TypeString);

	for (llvm::ImmutableSet<const AnnotatedExpr *>::iterator si =
			this->annotatedExpressions.begin(),
				 se = this->annotatedExpressions.end(); si != se; ++si)
	{
		string condition = "";
		if((*si)->negation)
		{
			condition += "!( ";
		}

		//pretty print the expr here
		(*si)->expression->printPretty(ss,0,policy);

		condition += ss.str();

		if((*si)->negation)
		{
			condition += " )";
		}
		conditions.push_back(condition);
	}

	return conditions;
}

llvm::ImmutableSet<const AnnotatedExpr *> ReachableConditions::Conditions::getAnnotatedExpressions() const {
	return this->annotatedExpressions;
}

bool ReachableConditions::Conditions::isReachableConditionsPresent() const {
	return !this->annotatedExpressions.isEmpty();
}

namespace {
	template <typename SET>
	// mergeset is going to be an intersection of the two sets for this analysis
	SET mergeSets(SET A, SET B, SET C) {
		if (A.isEmpty())
		  	return A;
		if (B.isEmpty())
		  	return B;
		for (typename SET::iterator it = B.begin(), ei = B.end(); it != ei; ++it) {
		  	if (A.contains(*it))
			  	C = C.add(*it);
		}
		return C;
	}
}

void ReachableConditions::Observer::anchor() { }

ReachableConditions::Conditions
ReachableConditionsImpl::merge(ReachableConditions::Conditions valsA,
                         ReachableConditions::Conditions valsB) {
  
	llvm::ImmutableSetRef<const AnnotatedExpr *>
    	DSetRefA(valsA.annotatedExpressions.getRootWithoutRetain(), DSetFact.getTreeFactory()),
    	DSetRefB(valsB.annotatedExpressions.getRootWithoutRetain(), DSetFact.getTreeFactory());

	llvm::ImmutableSetRef<const AnnotatedExpr *> DSetRefC = llvm::ImmutableSetRef<const AnnotatedExpr *>::getEmptySet(DSetFact.getTreeFactory());

	DSetRefA = mergeSets(DSetRefA, DSetRefB, DSetRefC);
  
	// asImmutableSet() canonicalizes the tree, allowing us to do an easy
	// comparison afterwards.
	return ReachableConditions::Conditions(DSetRefA.asImmutableSet());
}

bool ReachableConditions::Conditions::equals(const Conditions &V) const {
  return annotatedExpressions == V.annotatedExpressions;
}

//===----------------------------------------------------------------------===//
// Query methods.
//===----------------------------------------------------------------------===//
std::string ReachableConditions::getConditions(const Stmt *S, const char delim) const {
	return getImpl(impl).stmtsToConditions[S].getConditions(delim);
}

std::vector<std::string> ReachableConditions::getConditions(const Stmt *S) const {
	return getImpl(impl).stmtsToConditions[S].getConditions();
}

bool ReachableConditions::isReachableConditionsPresent(const Stmt *S) const {
	return getImpl(impl).stmtsToConditions[S].isReachableConditionsPresent();
}

llvm::ImmutableSet<const AnnotatedExpr *> ReachableConditions::getAnnotatedExpressions(const Stmt *S) const {
	return getImpl(impl).stmtsToConditions[S].getAnnotatedExpressions();
}

ReachableConditions::Conditions
ReachableConditionsImpl::addDefaultConditionToSet(const CFGBlock *block, ReachableConditions::Conditions val){
	val.annotatedExpressions = DSetFact.add(val.annotatedExpressions, blockConditionsForDefault[block]);
	return val;
}

ReachableConditions::Conditions
ReachableConditionsImpl::runOnBlock(const CFGBlock *block,
                              ReachableConditions::Conditions val,
                              ReachableConditions::Observer *obs) {

	// Update val first with the expression and negateflag if present
	if(const Expr *e = blockConditions[block]){
		blockAnnotExpr[block].expression = e;
		blockAnnotExpr[block].negation = negFlag[block];
		val.annotatedExpressions = DSetFact.add(val.annotatedExpressions, &blockAnnotExpr[block]);
	}

	// Apply the transfer function for all Stmts in the block.
	for (CFGBlock::const_reverse_iterator it = block->rbegin(),
		ei = block->rend(); it != ei; ++it)
	{
		const CFGElement &elem = *it;

		if (!elem.getAs<CFGStmt>())
			continue;

		const Stmt *S = elem.castAs<CFGStmt>().getStmt();
		stmtsToConditions[S] = val;
	}

	//Now we will populate the condition information
	if (CFGTerminator term = block->getTerminator())	//if we have a terminator in the block
	{
		if(Stmt *termstmt = term.getStmt())  			// a valid terminator
		{
			if (isa<IfStmt>(*termstmt))					// if the terminator is an if statement
			{
				IfStmt *is = cast<IfStmt>(termstmt);
				if(is != NULL )
				{
					if (const Expr *cond = is->getCond())
					{
						int ct = 0;
						std::string TypeString;
						llvm::raw_string_ostream ss(TypeString);
						cond->printPretty(ss,0,policy);
						//cout << "condition: " + ss.str() << endl;
						// TODO: unroll for loop as it is always have 2 iterations
						for (CFGBlock::const_succ_iterator I = block->succ_begin(),
									 E = block->succ_end(); I != E; ++I, ++ct)
						{
							if(ct == 0)
							{
								blockConditions[*I] = cond;
								negFlag[*I] = false;
							}
							else if((ct == 1) && (is->getElse()))
							{
								blockConditions[*I] = cond;
								negFlag[*I] = true;
							}
							else if ((ct == 1) && !(is->getElse()))
							{
								AnnotatedExpr * temp = new AnnotatedExpr();
								temp->expression = cond;
								temp->negation = true;
								blockConditionsForDefault[*I]=temp;
							}
						}
					}
				}
			}
		}
	}

	// TODO: add rest of the conditions (FOR, DO_WHILE, SWITCH)



	//code added here!
	if (CFGTerminator term = block->getTerminator())	//if we have a terminator in the block
	{
		if(Stmt *termstmt = term.getStmt())  			// a valid terminator
		{
			if(isa<WhileStmt>(*termstmt))
			{
				WhileStmt *ws = cast<WhileStmt>(termstmt);
				if(ws != NULL )
				{
					if(const Expr *cond = ws->getCond())
					{
						int ct=0;
						//string condition = cond->getAsString();

						std::string TypeString;
						llvm::raw_string_ostream ss(TypeString);

						LangOptions langopts;
						langopts.CPlusPlus = true;
						PrintingPolicy policy(langopts);
						//llvm::errs() << "Timer\n";
						//callback->printPretty(llvm::errs(), 0, policy);
						cond->printPretty(ss, 0, policy);
						//llvm::errs() << "\n -------------\n";
						// MCE->printPretty(llvm::errs(), 0, policy);
						//llvm::errs() << "\n -------------\n";

						std::string conditionStr = ss.str();

						if(!conditionStr.compare("ros::ok()") || !conditionStr.compare("ok()") || !conditionStr.compare("1") || !conditionStr.compare("true"))
							return val;

						for(CFGBlock::const_succ_iterator I = block->succ_begin(),
									E = block->succ_end(); I != E; ++I, ++ct)
						{
							if(ct==0)
							{
								blockConditions[*I]=cond;
								negFlag[*I]=false;
							}
//Only the loop part is condition reachable
//							if(ct==1)
//							{
//								blockConditions[*I]=cond;
//								negFlag[*I]=true;
//							}


						}

					}
				}
			}

      	if(isa<ForStmt>(*termstmt))
      	{
        	ForStmt *fs = cast<ForStmt>(termstmt);
        		if(fs != NULL ){
					if(const Expr *cond = fs->getCond())
					{
						int ct=0;
						for(CFGBlock::const_succ_iterator I = block->succ_begin(),
							E = block->succ_end(); I != E; ++I, ++ct)
            			{
							if(ct==0)
						  	{
								blockConditions[*I]=cond;
								negFlag[*I]=false;
              				}
//only loop part is reachable
//              			else if(ct==1)
//              			{
//              				blockConditions[*I]=cond;
//                				negFlag[*I]=true;
//              			}
            			}
          			}
        		}
      		}


			if(isa<DoStmt>(*termstmt))
			{
				DoStmt *ds = cast<DoStmt>(termstmt);
				if(ds != NULL ){
					if(const Expr *cond = ds->getCond())
					{
						int ct=0;
						for(CFGBlock::const_succ_iterator I = block->succ_begin(),
							E = block->succ_end(); I != E; ++I, ++ct)
						{
			  				if(ct==0)
			  				{
								blockConditions[*I]=cond;
								negFlag[*I]=false;
						  	}
//only the loop part gets the conditions
//			  				else if(ct==1)
//							{
//								blockConditions[*I]=cond;
//								negFlag[*I]=true;
//						  	}
						}
			  		}
				}
			}

			//only store the variable switched
			if(isa<SwitchStmt>(*termstmt))
			{
				SwitchStmt *ss = cast<SwitchStmt>(termstmt);
				if(ss != NULL ){
					if(const Expr *cond = ss->getCond())
					{
						int ct=0;
						for(CFGBlock::const_succ_iterator I = block->succ_begin(),
							E = block->succ_end(); I != E; ++I, ++ct)
						{
							//all cases are tainted
							blockConditions[*I]=cond;
							negFlag[*I]=false;
						}
			  		}
				}
			}
		}
	}




	return val;
}

void ReachableConditions::runOnAllBlocks(ReachableConditions::Observer &obs) {
  const CFG *cfg = getImpl(impl).analysisContext.getCFG();
  for (CFG::const_iterator it = cfg->begin(), ei = cfg->end(); it != ei; ++it)
    getImpl(impl).runOnBlock(*it, getImpl(impl).blocksEndToConditions[*it], &obs);
}

ReachableConditions::ReachableConditions(void *im) : impl(im) {}

ReachableConditions::~ReachableConditions() {
  delete (ReachableConditionsImpl*) impl;
}

ReachableConditions *
ReachableConditions::computeReachableConditions(AnalysisDeclContext &analysisContext) {

	// No CFG?  Bail out.
	CFG *cfg = analysisContext.getCFG();
	if (!cfg)
		return nullptr;

	// The analysis currently has scalability issues for very large CFGs.
	// Bail out if it looks too large.
	if (cfg->getNumBlockIDs() > 300000)
		return nullptr;

	ReachableConditionsImpl *RC = new ReachableConditionsImpl(analysisContext);


	// Construct the dataflow worklist.  Enqueue the entry block as the
	// start of the analysis.
	DataflowWorklist worklist(*cfg, analysisContext);
	llvm::BitVector everAnalyzedBlock(cfg->getNumBlockIDs());

    worklist.enqueueBlock(&(cfg->getEntry()));
	worklist.sortWorklist();
  
  	while (const CFGBlock *block = worklist.dequeue()) {
    	// Determine if the block's end value has changed.  If not, we
    	// have nothing left to do for this block.
    	Conditions &prevVal = RC->blocksBeginToConditions[block];

		//std::cerr << "\nBlock " << block->getBlockID() << std::endl;
    	// Merge the values of all predecessor blocks.
    	Conditions val;
    	Conditions merge_temp_val;
		bool firstPred = true;
		for (CFGBlock::const_pred_iterator it = block->pred_begin(),
					 ei = block->pred_end(); it != ei; ++it)
		{

			if (const CFGBlock *pred = *it)
			{
				//std::cerr << "Pred " << pred->getBlockID() << std::endl;
				if(firstPred)
				{
					firstPred = false; //merging should start after we have val
					// equal to the conditions from the first pred block

					val = RC->blocksEndToConditions[pred];

					//If pred is not null and the terminator of the previous block is
					// IfStmt and doesn't have an else part
					if(pred 												// pred is not null
					   && (pred->getTerminator().getStmt()) 				// there is a terminator in the previous block
					   && isa<IfStmt>(*(pred->getTerminator().getStmt()))) // that terminator is an IfStmt
					{
						//if there was no else part to the if condition then we add a custom negated condition for that block
						if ((RC->blockConditionsForDefault[block]) != NULL)
						{
							//std::cerr << " - in if condition 1- " << block->getBlockID() << std::endl;
							val = RC->addDefaultConditionToSet(block, val);
							/*LangOptions langOpts;
							langOpts.CPlusPlus = true;
							PrintingPolicy policy(langOpts);
							if((*val.annotatedExpressions.begin())->negation)
							{
								std::cerr << "Negation " ;
							}
							(*val.annotatedExpressions.begin())->expression->printPretty(llvm::errs(),0,policy);*/
						}
					}

				}
				else
				{
					merge_temp_val = RC->blocksEndToConditions[pred];
					//If pred is not null and the terminator of the previous block is
					// IfStmt and doesn't have an else part
					if(pred 												// pred is not null
						&& (pred->getTerminator().getStmt()) 				// there is a terminator in the previous block
						&& isa<IfStmt>(*(pred->getTerminator().getStmt()))) // that terminator is an IfStmt
					{
						//if there was no else part to the if condition then we add a custom negated condition for that block
						if ((RC->blockConditionsForDefault[block]) != NULL)
						{
							//std::cerr << " - in if condition 2- " << block->getBlockID() << std::endl;
							merge_temp_val = RC->addDefaultConditionToSet(block, merge_temp_val);
							/*LangOptions langOpts;
							langOpts.CPlusPlus = true;
							PrintingPolicy policy(langOpts);
							if((*merge_temp_val.annotatedExpressions.begin())->negation)
							{
								std::cerr << "Negation " ;
							}
							(*merge_temp_val.annotatedExpressions.begin())->expression->printPretty(llvm::errs(),0,policy);*/
						}
					}
					val = RC->merge(val, merge_temp_val);
					//std::cerr << val.annotatedExpressions.getHeight() << std::endl;
				}
			}
		}

		/*if(!val.annotatedExpressions.isEmpty())
		{
			std::cerr << " - out if condition - " << block->getBlockID() << std::endl;
			LangOptions langOpts;
			langOpts.CPlusPlus = true;
			PrintingPolicy policy(langOpts);
			std::cerr << "val";
			for(llvm::ImmutableSet<const AnnotatedExpr *>::iterator condit = val.annotatedExpressions.begin(),
						condend = val.annotatedExpressions.end(); condit != condend; ++condit){
				if((*condit)->negation)
				{
					std::cerr << "Negation " ;
				}
				(*condit)->expression->printPretty(llvm::errs(),0,policy);
			}
		}*/

		if (!everAnalyzedBlock[block->getBlockID()])
			everAnalyzedBlock[block->getBlockID()] = true;
		else if (prevVal.equals(val))
			continue;

		prevVal = val;

		// Update the dataflow value for the end of this block.
		RC->blocksEndToConditions[block] = RC->runOnBlock(block, val);

		// Enqueue the value to the successors.
		worklist.enqueueSuccessors(block);
  	}
  
  	return new ReachableConditions(RC);
}

void ReachableConditions::dumpBlockConditions(const SourceManager &M) {
  getImpl(impl).dumpBlockConditions(M);
}

void ReachableConditionsImpl::dumpBlockConditions(const SourceManager &M) {
	std::vector<const CFGBlock *> vec;
	for (std::map<const CFGBlock *, ReachableConditions::Conditions>::iterator
		it = blocksEndToConditions.begin(), ei = blocksEndToConditions.end();
		it != ei; ++it)
	{
		vec.push_back(it->first);
	}

	std::sort(vec.begin(), vec.end(), [](const CFGBlock *A, const CFGBlock *B)
	{
		return A->getBlockID() < B->getBlockID();
	});

	std::vector<const AnnotatedExpr*> expressions;
//	std::cerr << __LINE__ << std::endl;
	for (std::vector<const CFGBlock *>::iterator
		it = vec.begin(), ei = vec.end(); it != ei; ++it)
	{


		llvm::errs() << "\n[ B" << (*it)->getBlockID()
			<< " (reachable conditions at block exit) ]\n";

//		std::cerr << __LINE__ << std::endl;
		ReachableConditions::Conditions vals = blocksEndToConditions[*it];
		expressions.clear();
//
//		std::cerr << __LINE__ << std::endl;

		for (llvm::ImmutableSet<const AnnotatedExpr *>::iterator si =
			vals.annotatedExpressions.begin(),
			se = vals.annotatedExpressions.end(); si != se; ++si)
		{
//			std::cerr << __LINE__ << std::endl;
		/*	const AnnotatedExpr *ae = *si;*/
			/*printf("add %p\n", (void*)((ae)));
			printf("add %p\n", (void*)((ae)->expression));*/

			/*void* ptr = reinterpret_cast<void*>(103079215104); //12 gb equivalent as my system only has 12 gb ram
			if(((void*)ae-> expression) >= ptr) continue;*/

		/*	if((!ae) || (!(ae)->expression) || !(strcmp((*ae).expression->getStmtClassName(), "UnaryTypeTraitExpr")) || (((*ae).expression->getStmtClass()) == Stmt::NoStmtClass)) {
				std::cerr << __LINE__ << std::endl;
				continue;
			}*/

			expressions.push_back(*si);
		}

	/*	std::sort(expressions.begin(), expressions.end(), [](const AnnotatedExpr *A, const AnnotatedExpr *B)
		{
			SourceLocation ALoc = A->expression->getLocStart();
			SourceLocation BLoc = B->expression->getLocStart();
			return ALoc.getRawEncoding() < BLoc.getRawEncoding();
			//return A->expression->getLocStart() < B->expression->getLocStart();
		});*/

		for (std::vector<const AnnotatedExpr *>::iterator di = expressions.begin(),
			de = expressions.end(); di != de; ++di)
		{
			if(!(*di)->expression)
				continue;
//			std::cerr << __LINE__ << std::endl;
			if(!isa<Stmt>(*((*di)->expression)))
				continue;
			/*if(!(strcmp((*di)->expression->getStmtClassName(), "UnaryTypeTraitExpr"))
			   || !(strcmp((*di)->expression->getStmtClassName(), "CXXPseudoDestructorExpr")))
				continue;*/
//			std::cerr << __LINE__ << std::endl;
			if((*di)->negation)
			{
				llvm::errs() << " !(";
			}

			llvm::errs() << " ";

			//pretty print the expr here
			(*di)->expression->printPretty(llvm::errs(),0,policy);
//			llvm::errs() << "\n";

			if((*di)->negation)
			{
				llvm::errs() << " )";
			}


			//printf("add %p\n", (void*)(*di)->expression);
			//(*di)->expression->getExprLoc().dump(M); // print location of the expression

//			llvm::errs() << ">\n";
			llvm::errs() << "\n";
		}
	}
	llvm::errs() << "\n";
}

const void *ReachableConditions::getTag() { static int x; return &x; }
