//===- ReachableConditions.h - Reachable Conditions Analysis for Source CFGs -*- C++ --*-//
//
//
//
//===----------------------------------------------------------------------===//
//
// This file implements Reachable Conditions analysis for source-level CFGs.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_ANALYSIS_ANALYSES_ReachableConditions_H
#define LLVM_CLANG_ANALYSIS_ANALYSES_ReachableConditions_H

#include "clang/AST/Decl.h"
#include "clang/Analysis/AnalysisContext.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ImmutableSet.h"
#include "string"
#include "vector"
#include <iostream>

using namespace std;

namespace clang
{
	class CFG;
	class CFGBlock;
	class Stmt;
	class DeclRefExpr;
	class SourceManager;

//	typedef struct AnnotExpr
	class AnnotatedExpr
	{
	public:
		const Expr* expression;
		bool negation;

		//code added for making it a class
		AnnotatedExpr() {
		}
		AnnotatedExpr(const AnnotatedExpr &other)
		{
			this->expression = other.expression;
			this->negation = other.negation;
		}

		AnnotatedExpr& operator = (const AnnotatedExpr &other)
		{
			if(this != &other)
			{
				this->expression = other.expression;
				this->negation = other.negation;
			}
			return *this;
		}

		virtual ~AnnotatedExpr()
		{
		}

	};
	//AnnotatedExpr;

	class ReachableConditions : public ManagedAnalysis
	{
	public:
		class Conditions {
		public:

			llvm::ImmutableSet<const AnnotatedExpr *> annotatedExpressions;

			bool equals(const Conditions &V) const;

			Conditions()
				: annotatedExpressions(nullptr) {}

			Conditions(llvm::ImmutableSet<const AnnotatedExpr *> annotExpressions)
				: annotatedExpressions(annotExpressions) {}

			std::string getConditions(const char delim) const;
			std::vector<std::string> getConditions() const;
			bool isReachableConditionsPresent() const;
			llvm::ImmutableSet<const AnnotatedExpr *> getAnnotatedExpressions() const;

			friend class ReachableConditions;
		};

		class Observer
		{
			virtual void anchor();
		public:
			virtual ~Observer() {}

			/// A callback invoked right before invoking the
			///  reachability transfer function on the given statement.
			virtual void observeStmt(const Stmt *S,
									 const CFGBlock *currentBlock,
									 const Conditions& V) {}
		};

		virtual ~ReachableConditions() override;

		/// Compute the reachable conditions information for a given CFG.
		static ReachableConditions *computeReachableConditions(AnalysisDeclContext &analysisContext);

		/// Returns a concatenated string of conditions delimited with the char argument for any given statement
		/// in the CFG.
		std::string getConditions(const Stmt *S, const char delim) const;

		/// Returns a vector of conditions in string form for any given statement
		/// in the CFG.
		std::vector<std::string> getConditions(const Stmt *S) const;

		/*
		 * returns true if the stmt has reachable conditions
		 */
		bool isReachableConditionsPresent(const Stmt *S) const;

		/// Returns a set of Conditions in their original AnnotatedExpr form
		llvm::ImmutableSet<const AnnotatedExpr *> getAnnotatedExpressions(const Stmt *S) const;

		/// Print to stderr the Reachability information associated with
		/// each basic block.
		void dumpBlockConditions(const SourceManager& M);

		void runOnAllBlocks(Observer &obs);

		static ReachableConditions *create(AnalysisDeclContext &analysisContext) {
			return computeReachableConditions(analysisContext);
		}

		static const void *getTag();

	private:
		ReachableConditions(void *impl);
		void *impl;
	};


} // end namespace clang

#endif
