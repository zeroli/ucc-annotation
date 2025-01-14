#ifndef __GEN_H_
#define __GEN_H_

enum OPCode
{
#define OPCODE(code, name, func) code,
#include "opcode.h"
#undef OPCODE
};

typedef struct irinst
{
	// 所有指令形成双向链表
	struct irinst *prev;
	struct irinst *next;
	// 指令操作结果数据类型
	Type ty;
	// 3地址码操作指令
	int opcode;
	// [0]: dest, [1]/[2]: src
	Symbol opds[3];
} *IRInst;

typedef struct cfgedge
{
	BBlock bb;
	struct cfgedge *next;
} *CFGEdge;

struct bblock
{
	// 所有的block形成一个双向链表
	struct bblock *prev;
	struct bblock *next;
	Symbol sym;
	// 每个block有前驱和后继edges
	// 这些edges形成单向链表
	CFGEdge succs;
	CFGEdge preds;
	// block当然有指令序列，双向链表
	// `insth`是头，从而形成双向循环链表
	struct irinst insth;
	int ninst;
	int nsucc;
	int npred;
	int ref;
	int accessed;
	int *dom;
	void *priv;
};

typedef struct ilarg
{
	Symbol sym;
	Type ty;
} *ILArg;

BBlock CreateBBlock(void);
void   StartBBlock(BBlock bb);

void GenerateMove(Type ty, Symbol dst, Symbol src);
void GenerateIndirectMove(Type ty, Symbol dst, Symbol src);
void GenerateAssign(Type ty, Symbol dst, int opcode, Symbol src1, Symbol src2);
void GenerateInc(Type ty, Symbol src);
void GenerateDec(Type ty, Symbol src);
void GenerateBranch(Type ty, BBlock dstBB, int opcode, Symbol src1, Symbol src2);
void GenerateJump(BBlock dstBB);
void GenerateIndirectJump(BBlock *dstBBs, int len, Symbol index);
void GenerateReturn(Type ty, Symbol src);
void GenerateFunctionCall(Type ty, Symbol recv, Symbol faddr, Vector args);
void GenerateClear(Symbol dst, int size);

void DefineTemp(Symbol t, int op, Symbol src1, Symbol src2);
Symbol AddressOf(Symbol sym);
Symbol Deref(Type ty, Symbol addr);
Symbol TryAddValue(Type ty, int op, Symbol src1, Symbol src2);
Symbol Simplify(Type ty, int op, Symbol src1, Symbol src2);

void DrawCFGEdge(BBlock head, BBlock tail);
void ExamineJump(BBlock bb);
BBlock TryMergeBBlock(BBlock bb1, BBlock bb2);
void Optimize(FunctionSymbol fsym);

extern BBlock CurrentBB;
extern int OPMap[];

#endif
