/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/**
 * \file
 *
 * \brief Implementation file for clang::ExprScopeAnalysis
 *
 * \author Nishant Sharma
 *
 * \version 0.5
 *
 * \date 19th June, 2016
 *
 * Contact: nsharma@cse.unl.edu
 */

#include "clang/Analysis/Analyses/ExprScopeAnalysis.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Analysis/Analyses/PostOrderCFGView.h"
#include "clang/Analysis/AnalysisContext.h"
#include "clang/Analysis/CFG.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <vector>
#include <clang/AST/PrettyPrinter.h>

#define DEBUG ;
//llvm::errs() << __LINE__ << "\n" ;


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
	/**
	 * \class
	 *
	 * \brief This is the main implementation class for the analysis
	 *
	 */
	class ExprScopeAnalysisImpl {
	public:
		AnalysisDeclContext &analysisContext;
		std::map<const CFGBlock *, ExprScopeAnalysis::LocalScopeValues> blocksEndToLocalness;
		std::map<const CFGBlock *, ExprScopeAnalysis::LocalScopeValues> blocksBeginToLocalness;

		///Merge two blocks information into one, this is a 'union' merge
		ExprScopeAnalysis::LocalScopeValues
				merge(ExprScopeAnalysis::LocalScopeValues valsA,
					  ExprScopeAnalysis::LocalScopeValues valsB);

		///this function acts a gen,kill function for a block
		ExprScopeAnalysis::LocalScopeValues
				runOnBlock(const CFGBlock *block, ExprScopeAnalysis::LocalScopeValues val);

		///to dump the generated information on llvm::errs()
		void dumpBlockLocalness(const SourceManager& M);

		///Basic constructor
		ExprScopeAnalysisImpl(AnalysisDeclContext &ac)
				: analysisContext(ac)
		{}
	};
}

static ExprScopeAnalysisImpl &getImpl(void *x) {
	return *((ExprScopeAnalysisImpl *) x);
}

//===----------------------------------------------------------------------===//
// Operations and queries on LocalScopeValues.
//===----------------------------------------------------------------------===//

bool ExprScopeAnalysis::LocalScopeValues::isLocal(const Expr *expr) const {
	if(localExpr.find(expr) != localExpr.end()){
		return localExpr.at(expr);
	}
	return GLOBAL;
}

bool ExprScopeAnalysis::LocalScopeValues::isLocal(const VarDecl *varDecl) const {
	if(localVarDecl.find(varDecl) != localVarDecl.end()){
		return localVarDecl.at(varDecl);
	}
	return GLOBAL;
}

/**
 * \brief Merge two Local Scope Values in a Union operator manner
 *
 * if the value presents in both of them and one of them differs from
 * the other, output prefers global over local
 */
ExprScopeAnalysis::LocalScopeValues
ExprScopeAnalysisImpl::merge(ExprScopeAnalysis::LocalScopeValues valsA,
						 ExprScopeAnalysis::LocalScopeValues valsB) {
	//to keep the result values
	ExprScopeAnalysis::LocalScopeValues mergedSet;

	for (auto setAIt = valsA.localExpr.cbegin(); setAIt != valsA.localExpr.cend(); ++setAIt) {
		//if exists in B, check for their values
		auto setBIt = valsB.localExpr.find(setAIt->first);
		if(setBIt != valsB.localExpr.cend()) {
			if(setAIt->second == setBIt->second)
			{
				//if the expressions match and have the same
				mergedSet.localExpr[setAIt->first] = setAIt->second;
			}
			else
			{
				//we will assign GLOBAL (since there are two values one is global if they mismatch
				//FIXME: write a resolver which assigns the most priority value of the key (only required after we add more than two value options
				mergedSet.localExpr[setAIt->first] = ExprScopeAnalysis::GLOBAL;
			}
			/*
			 * Delete this entry from B for two reasons. searching the next entry will be fast and after this when we append B to the set it will be necessary
			 */
			valsB.localExpr.erase(setBIt);
		}
		//if we don't find the element in B, just add it
		else{
			mergedSet.localExpr[setAIt->first] = setAIt->second;
		}
	}
	//any remaining entry in B will just be added as it is
	for (auto setBIt = valsB.localExpr.cbegin(); setBIt != valsB.localExpr.cend(); ++setBIt) {
		mergedSet.localExpr[setBIt->first] = setBIt->second;
	}

	/*
	 * Now we do the same for local vardecls
	 */
	for (auto setAIt = valsA.localVarDecl.cbegin(); setAIt != valsA.localVarDecl.cend(); ++setAIt) {
		//if exists in B, check for their values
		auto setBIt = valsB.localVarDecl.find(setAIt->first);
		if(setBIt != valsB.localVarDecl.cend())
		{
			if(setAIt->second == setBIt->second)
			{
				//if the experssions match and have the same
				mergedSet.localVarDecl[setAIt->first] = setAIt->second;
			}
			else
			{
				//we will assign GLOBAL (since there are two values one is global if they mismatch
				//FIXME: write a resolver which assigns the most priority value of the key (only required after we add more than two value options
				mergedSet.localVarDecl[setAIt->first] = ExprScopeAnalysis::GLOBAL;
			}
			/*
			 * Delete this entry from B for two reasons. searching the next entry will be fast and after this when we append B to the set it will be nessesary
			 */
			valsB.localVarDecl.erase(setBIt);
		}
			//if we don't find the element in B, just add it
		else{
			mergedSet.localVarDecl[setAIt->first] = setAIt->second;
		}
	}
	//any remaining entry in B will just be added as it is
	for (auto setBIt = valsB.localVarDecl.cbegin(); setBIt != valsB.localVarDecl.cend(); ++setBIt) {
		mergedSet.localVarDecl[setBIt->first] = setBIt->second;
	}

	return mergedSet;
}

//===----------------------------------------------------------------------===//
// Query methods.
//===----------------------------------------------------------------------===//

bool ExprScopeAnalysis::isLocal(const CFGBlock *block, const VarDecl *varDecl) {
	return getImpl(implementation).blocksEndToLocalness[block].isLocal(varDecl);
}

bool ExprScopeAnalysis::isLocal(const CFGBlock *block, const Expr *expr) {
	return getImpl(implementation).blocksEndToLocalness[block].isLocal(expr);
}

bool ExprScopeAnalysis::LocalScopeValues::equals(const LocalScopeValues &localScopeValues) const {

	//if the sizes are different they are not equal
	if(localExpr.size() != localScopeValues.localExpr.size() || localVarDecl.size() != localScopeValues.localVarDecl.size()){
		return false;
	}
	//check for whether they are both equal or not
	bool equal = true;
	//now make sure they are equal
	for (auto setAIt = this->localExpr.cbegin(); setAIt != localExpr.cend(); ++setAIt) {
		//if exists in B, check for their values
		auto setBIt = localScopeValues.localExpr.find(setAIt->first);
		if(setBIt != localScopeValues.localExpr.cend()) {
			if (setAIt->second != setBIt->second) {
				equal = false;
				break;
			}
		}
		else{
			equal = false;
			break;
		}
	}

	//now make sure they are equal
	for (auto setAIt = this->localVarDecl.cbegin(); setAIt != localVarDecl.cend(); ++setAIt) {
		//if exists in B, check for their values
		auto setBIt = localScopeValues.localVarDecl.find(setAIt->first);
		if(setBIt != localScopeValues.localVarDecl.cend()) {
			if (setAIt->second != setBIt->second) {
				equal = false;
				break;
			}
		}
		else{
			equal = false;
			break;
		}
	}

	return equal;
}

//===----------------------------------------------------------------------===//
// Dataflow computation.
//===----------------------------------------------------------------------===//

namespace {
	/*
	 * TODO: instead of using StmtVisitor call the visitor yourself using the const * rather than stmt visitor's
	 */
	/**
	 * \class
	 *
	 * \brief This is the class which populates the gen set
	 *
	 * It's important to not touch the member variables of the analysis but we should work with the localScopeValues
	 *
	 * LocalScopeValues has the merged information already and is used as the gen/kill set for this analysis
	 */
	class TransferFunctions : public StmtVisitor<TransferFunctions> {
		ExprScopeAnalysisImpl &exprScopeAnalysis;
		ExprScopeAnalysis::LocalScopeValues &localScopeValues;
		const CFGBlock *currentBlock;
	public:
		TransferFunctions(ExprScopeAnalysisImpl &scopeAnalysis,
						  ExprScopeAnalysis::LocalScopeValues &scopeValues,
						  const CFGBlock *cfgBlock)
				: exprScopeAnalysis(scopeAnalysis), localScopeValues(scopeValues), currentBlock(cfgBlock) {}

		//for localVarDecls
		void VisitBinaryOperator(BinaryOperator *binaryOperator);
		void VisitDeclStmt(DeclStmt * declStmt);
		void VisitCallExpr(CallExpr *callExpr);

		//for localExpr
		void VisitTerminator(Stmt* stmt);

		ExprScopeAnalysis::Scope HandleExpr(Expr *expr);
		ExprScopeAnalysis::Scope HandleBinaryOp(BinaryOperator* binaryOperator);
		ExprScopeAnalysis::Scope HandleAssignmentOp(BinaryOperator* binaryOperator);
	};
}

ExprScopeAnalysis::Scope TransferFunctions::HandleBinaryOp(BinaryOperator* binaryOperator){
	if(binaryOperator->isAssignmentOp()){
		return HandleAssignmentOp(binaryOperator);
	}
	else
	{
		Expr *LHS = binaryOperator->getLHS()->IgnoreParens();
		Expr *RHS = binaryOperator->getRHS()->IgnoreParens();

		if(!LHS && !RHS) return ExprScopeAnalysis::LOCAL;

		ExprScopeAnalysis::Scope resultLHS = HandleExpr(LHS);
		ExprScopeAnalysis::Scope resultRHS = HandleExpr(RHS);

		if(resultLHS == ExprScopeAnalysis::GLOBAL || resultRHS == ExprScopeAnalysis::GLOBAL)
			return ExprScopeAnalysis::GLOBAL;
		else
			return ExprScopeAnalysis::LOCAL;
	}
}

ExprScopeAnalysis::Scope TransferFunctions::HandleAssignmentOp(BinaryOperator* binaryOperator){
	//if b doesn't exist local else global in the end if nothing else is found
	if(!binaryOperator) return ExprScopeAnalysis::LOCAL;

	if (binaryOperator->isAssignmentOp()) {
		// Assigning to a variable?
		if(Expr *LHS = binaryOperator->getLHS()->IgnoreParens()){
			if (DeclRefExpr *DR = dyn_cast<DeclRefExpr>(LHS)){
				if (VarDecl *VD = dyn_cast<VarDecl>(DR->getDecl())) {
					// only move forward if the variable has a local storage
					if(VD->hasLocalStorage())
					{
						/*
						 * now let's parse the right hand side
						 */
						Expr *RHS = binaryOperator->getRHS()->IgnoreParens();
						ExprScopeAnalysis::Scope result = HandleExpr(RHS);
						auto it = localScopeValues.localVarDecl.find(VD);
						if(it == localScopeValues.localVarDecl.end()){ //if doesn't exist add
							localScopeValues.localVarDecl[VD] = result;
						}
						//otherwise
						else {
							if(result == ExprScopeAnalysis::GLOBAL){
								localScopeValues.localVarDecl[VD] = result;
							}
							//for result is local we will keep the current value
						}
						return result;
					}
				}
			}
		}
	}
	else{
		return HandleBinaryOp(binaryOperator);
	}
	return ExprScopeAnalysis::GLOBAL;
}

ExprScopeAnalysis::Scope TransferFunctions::HandleExpr(Expr *expr) {

	// if it's simply  a variable this will also take care of the memberexpr we are only taking care of the variable
	// (member variables are attached to their objects)
	// sometimes a Decl ref get casted so we extract the declref out of the cast
	if(ImplicitCastExpr * implicitCastExpr = dyn_cast<ImplicitCastExpr>(expr))
	{
		if(isa<DeclRefExpr>(implicitCastExpr->getSubExpr())){
			expr = implicitCastExpr->getSubExpr();
		}
	}
	if(DeclRefExpr* dre = dyn_cast<DeclRefExpr>(expr)){
		if(ParmVarDecl* varDecl = dyn_cast<ParmVarDecl>(dre->getDecl())) {
			if (localScopeValues.localVarDecl.find(varDecl) != localScopeValues.localVarDecl.end())
				return localScopeValues.localVarDecl[varDecl];
			else {//add the ParamVarDecl to the list and return
				localScopeValues.localVarDecl[varDecl] = ExprScopeAnalysis::LOCAL;
				return ExprScopeAnalysis::LOCAL;
			}
		}
		if(VarDecl* varDecl = dyn_cast<VarDecl>(dre->getDecl())){
			if(localScopeValues.localVarDecl.find(varDecl) != localScopeValues.localVarDecl.end())
				return localScopeValues.localVarDecl[varDecl];
		}
	}


	//if it's a callexpr (cxxmembercallexpr should be taken care from here)
	if (CallExpr *callExpr = dyn_cast<CallExpr>(expr)) {
		if (FunctionDecl *functionDecl = callExpr->getDirectCallee()) {
			/**
			 * if any local vardecl is passed to any function as a reference make it global
			*/
			int paramNo = 0;
			for (CallExpr::arg_iterator it = callExpr->arg_begin(); it != callExpr->arg_end(); ++it) {
				///if argument is a local vardecl
				if (DeclRefExpr *param = dyn_cast<DeclRefExpr>(*it)) {
					//access the variable
					if (VarDecl *vd = dyn_cast<VarDecl>(param->getDecl())) {
						//if the passed value is a local variable
						if (vd->hasLocalStorage()) {
							//check if the param is a reference of a copy
							ParmVarDecl *parmVarDecl = functionDecl->getParamDecl(paramNo);
							if (parmVarDecl) {
								if (parmVarDecl->getType()->isReferenceType() ||
									parmVarDecl->getType()->isPointerType()) {
									localScopeValues.localVarDecl[vd] = ExprScopeAnalysis::GLOBAL;
								}
							}
						}
					}
				}
				++paramNo;
			}
		}
		//if it's a callExpr we will conservatively return GLOBAL
		return ExprScopeAnalysis::GLOBAL;
	}

	//if it's a constructor get it's decl and let callexpr handle it
	//FIXME: extract this and the aboce one to make a function
	if(CXXConstructExpr * cxxConstructExpr = dyn_cast<CXXConstructExpr>(expr)){
		bool isGLobal = false;
		if(FunctionDecl * functionDecl = dyn_cast<FunctionDecl>(cxxConstructExpr->getConstructor())){
			/**
		    * if any local vardecl is passed to any function as a reference make it global
    		*/
			int paramNo = 0;
			for (CXXConstructExpr::arg_iterator it = cxxConstructExpr->arg_begin(); it != cxxConstructExpr->arg_end() ; ++it) {
				///if argument is a local vardecl
				if( DeclRefExpr *param = dyn_cast<DeclRefExpr>(*it)){
					//access the variable
					if( VarDecl* vd = dyn_cast<VarDecl>(param->getDecl())){
						//if the passed value is a local variable
						if(vd->hasLocalStorage()){
							//check if the param is a reference of a copy
							 ParmVarDecl * parmVarDecl = functionDecl->getParamDecl(paramNo);
							if(parmVarDecl)
							{
								if(parmVarDecl->getType()->isReferenceType() ||
										parmVarDecl->getType()->isPointerType()){
									localScopeValues.localVarDecl[vd] = ExprScopeAnalysis::GLOBAL;
								}
							}
							//if reference then set to global otherwise nothing
						}
						//if the passed variable has a global storage
						else{
							isGLobal = true;
						}
					}
				}
				++paramNo;
			}
		}
		if(isGLobal)
			return ExprScopeAnalysis::GLOBAL;
		else
			return ExprScopeAnalysis::LOCAL;
	}

	//now the binaryOperators statements
	if(BinaryOperator *binaryOperator = dyn_cast<BinaryOperator>(expr)){
		if(binaryOperator->isAssignmentOp()){
			return HandleAssignmentOp(binaryOperator);
		}
		else {
			return HandleBinaryOp(binaryOperator);
		}
	}

	//rest is local : to address bool, int , string literals and what not TOO MANY CASES
	return ExprScopeAnalysis::LOCAL;

}

void TransferFunctions::VisitDeclStmt(DeclStmt *declStmt) {
	if(!declStmt) return;
	for (auto *DI : declStmt->decls())
		if (VarDecl *varDecl = dyn_cast<VarDecl>(DI)) {
			//if it has a initializer attached otherwise local
			if(Expr * expr = varDecl->getInit())
				localScopeValues.localVarDecl[varDecl] = HandleExpr(expr);
			else
			{
				auto it = localScopeValues.localVarDecl.find(varDecl);
				if(it == localScopeValues.localVarDecl.end()){
					localScopeValues.localVarDecl[varDecl] = ExprScopeAnalysis::LOCAL;
				}
				//otherwise
				else {
					//TODO: revert this to print
					//llvm::errs() << localScopeValues.localVarDecl[varDecl] << "\n";
					;
				}
			}
		}
}

void TransferFunctions::VisitBinaryOperator(BinaryOperator *binaryOperator){
	if(!binaryOperator) return;
	if(binaryOperator->isAssignmentOp()){
		HandleAssignmentOp(binaryOperator);
	}
	else {
		HandleBinaryOp(binaryOperator);
	}
}

void TransferFunctions::VisitCallExpr(CallExpr *callExpr){
	HandleExpr(callExpr);
}

void TransferFunctions::VisitTerminator(Stmt* stmt){
	if(IfStmt* ifStmt = dyn_cast<IfStmt>(stmt)) {
		localScopeValues.localExpr[ifStmt->getCond()] = HandleExpr(ifStmt->getCond());
		return;
	}
	if(WhileStmt* whileStmt = dyn_cast<WhileStmt>(stmt)) {
		localScopeValues.localExpr[whileStmt->getCond()] = HandleExpr(whileStmt->getCond());
		return;
	}
	if(DoStmt* doStmt = dyn_cast<DoStmt>(stmt)) {
		localScopeValues.localExpr[doStmt->getCond()] = HandleExpr(doStmt->getCond());
		return;
	}
	if(SwitchStmt* switchStmt = dyn_cast<SwitchStmt>(stmt)) {
		localScopeValues.localExpr[switchStmt->getCond()] = HandleExpr(switchStmt->getCond());
		return;
	}
}


ExprScopeAnalysis::LocalScopeValues
ExprScopeAnalysisImpl::runOnBlock(const CFGBlock *block,
							  ExprScopeAnalysis::LocalScopeValues val) {

	TransferFunctions TF(*this, val, block);

	// Apply the transfer function for all Stmts in the block.
	for (CFGBlock::const_iterator it = block->begin(),
				 ei = block->end(); it != ei; ++it) {
		const CFGElement &elem = *it;

		if (!elem.getAs<CFGStmt>())
			continue;

		const Stmt *S = elem.castAs<CFGStmt>().getStmt();

		if(isa<CallExpr>(S) || isa<BinaryOperator>(S) || isa<DeclStmt>(S))
			TF.Visit(const_cast<Stmt*>(S));
	}

	// Visit the terminator (if any).
	if (const Stmt *term = block->getTerminator())
		TF.VisitTerminator(const_cast<Stmt*>(term));

	return val;
}

ExprScopeAnalysis::ExprScopeAnalysis(void *im) : implementation(im) {}

ExprScopeAnalysis::~ExprScopeAnalysis() {
	delete (ExprScopeAnalysisImpl*) implementation;
}


ExprScopeAnalysis *
ExprScopeAnalysis::computeLocalness(AnalysisDeclContext &AC) {

	// No CFG?  Bail out.
	CFG *cfg = AC.getCFG();
	if (!cfg)
		return nullptr;

	// The analysis currently has scalability issues for very large CFGs.
	// Bail out if it looks too large.
	if (cfg->getNumBlockIDs() > 300000)
		return nullptr;

	ExprScopeAnalysisImpl *LV = new ExprScopeAnalysisImpl(AC);

	// Construct the dataflow worklist.  Enqueue the entry block as the
	// start of the analysis.
	DataflowWorklist worklist(*cfg, AC);
	llvm::BitVector everAnalyzedBlock(cfg->getNumBlockIDs());
	worklist.enqueueBlock(&(cfg->getEntry()));
	worklist.sortWorklist();

	while (const CFGBlock *block = worklist.dequeue()) {
		// Determine if the block's end value has changed.  If not, we
		// have nothing left to do for this block.
		ExprScopeAnalysis::LocalScopeValues &prevVal = LV->blocksBeginToLocalness[block];

		// Merge the values of all pred blocks.
		ExprScopeAnalysis::LocalScopeValues val;
		for (CFGBlock::const_pred_iterator it = block->pred_begin(),
					 ei = block->pred_end(); it != ei; ++it) {
			if (const CFGBlock *pred = *it) {
				val = LV->merge(val, LV->blocksEndToLocalness[pred]);
				}
		}

		if (!everAnalyzedBlock[block->getBlockID()])
			everAnalyzedBlock[block->getBlockID()] = true;
		else if (prevVal.equals(val)){
			continue;
		}

		prevVal = val;

		// Update the dataflow value for the start of this block.
		LV->blocksEndToLocalness[block] = LV->runOnBlock(block, val); //val already has everything

		// Enqueue the value to the predecessors.
		worklist.enqueueSuccessors(block);
	}

	return new ExprScopeAnalysis(LV);
}

void ExprScopeAnalysis::dumpBlockLocalness(const SourceManager &M) {
	getImpl(implementation).dumpBlockLocalness(M);
}

void ExprScopeAnalysisImpl::dumpBlockLocalness(const SourceManager &M) {
	std::vector<const CFGBlock *> vec;
	for (std::map<const CFGBlock *, ExprScopeAnalysis::LocalScopeValues>::iterator
				 it = blocksEndToLocalness.begin(), ei = blocksEndToLocalness.end();
		 it != ei; ++it) {
		vec.push_back(it->first);
	}
	std::sort(vec.begin(), vec.end(), [](const CFGBlock *A, const CFGBlock *B) {
		return A->getBlockID() < B->getBlockID();
	});


	for (std::vector<const CFGBlock *>::iterator
				 it = vec.begin(), ei = vec.end(); it != ei; ++it) {
		llvm::errs() << "\n[ B" << (*it)->getBlockID()
		<< " (local/global affected variables at block exit) ]\n";

		ExprScopeAnalysis::LocalScopeValues vals = blocksEndToLocalness[*it];

		for (std::map<const VarDecl*, ExprScopeAnalysis::Scope>::iterator di = vals.localVarDecl.begin(),
					 de = vals.localVarDecl.end(); di != de; ++di) {
			llvm::errs() << " " << (di)->first->getDeclName().getAsString()
			<< " < " << (di)->second;
			llvm::errs() << " >\n";
		}

		if(!vals.localExpr.empty())
			llvm::errs() << "\n Now Expressions \n" ;

		for (auto di = vals.localExpr.begin(), de = vals.localExpr.end(); di != de; ++di) {
			llvm::errs() << " " ;
			LangOptions lo;
			lo.CPlusPlus = true;
			PrintingPolicy policy(lo);
			(di)->first->printPretty(llvm::errs(), 0, policy);
			llvm::errs() << " < " << (di)->second;
			llvm::errs() << " >\n";
		}

	}
	llvm::errs() << "\n";

}

const void *ExprScopeAnalysis::getTag() { static int x; return &x; }
