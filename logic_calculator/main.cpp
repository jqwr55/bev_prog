#include <fcntl.h>
#include <common.h>

enum TokenType : u32 {
    TOKEN_ERROR,

    TOKEN_NEGATION,     // !
    TOKEN_CONJUNCTION,  // &
    TOKEN_DISJUNCTION,  // |
    TOKEN_IMPLICATION,  // >
    TOKEN_EQUIVALENCE,  // ==
    TOKEN_EQUAL,        // =

    TOKEN_IDENTIFIER,
    TOKEN_BOOL_LITERAL,

    TOKEN_OPEN_PAREN,   // (,
    TOKEN_CLOSE_PAREN,  // )
    TOKEN_SEMICOLON,    // ;

    TOKEN_KEYWORD_EXIT,     // exit
    TOKEN_KEYWORD_CLEAR,    // clear
    TOKEN_EOF,

    TOKEN_COUNT,
};
constexpr const char* TOKEN_STRS[] = {
    "ERROR",
    "!",
    "&",
    "|",
    ">",
    "==",
    "=",
    "identifier",
    "bool literal",
    "(",
    ")",
    ";",
    "exit",
    "clear",
};
struct Token {
    TokenType type;
    u32 begin;
    u32 size;
};
struct Tokenizer {
    const char* begin;
    const char* end;
    const char* at;
};
struct Symbol {
    Token name;
    bool value;
};
struct Context {
    const char* name;
    Tokenizer tokenizer;
    Token* tokenStream;
    byte* exprBase;
    Symbol* environment;
    u32 environmentSize;
    u32 environmentCap;
    u32 exprCount;
};
enum ContextStatus : u32 {
    CTX_EXIT,
    CTX_EOF,
};
bool IsNumeric(char c) {
    return c >= '0' && c <= '9';
}
bool IsAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool TokenEquals(const char* begin, Token t, const char* match) {
    
    auto ptr = begin + t.begin;
    auto end = ptr + t.size;
    for(;;) {
        if(!*match) return ptr == end;
        if(ptr == end) return *match == 0;
        if(*ptr++ != *match++) return false;
    }
}
struct TokenKeyWord {
    TokenType type;
    const char* name;
};

constexpr TokenKeyWord keyWords[] = {
    {TOKEN_BOOL_LITERAL,       "false"},
    {TOKEN_BOOL_LITERAL,       "true"},
    {TOKEN_KEYWORD_CLEAR,      "clear"},
    {TOKEN_KEYWORD_EXIT,        "exit"},
};
Token GetToken(Tokenizer* tokenizer) {

    auto it = tokenizer->at;

    //skip white space
    while(*it != 0 && (*it == ' ' || *it == '\n')) it++;


    Token token;
    token.begin = it - tokenizer->begin;

    switch(*it) {
    case 0:
        token.type = TOKEN_EOF;
        break;
    case ';':
        token.type = TOKEN_SEMICOLON;
        break;
    case '(':
        token.type = TOKEN_OPEN_PAREN;
        break;
    case ')':
        token.type = TOKEN_CLOSE_PAREN;
        break;
    case '>':
        token.type = TOKEN_IMPLICATION;
        break;
    case '|':
        token.type = TOKEN_DISJUNCTION;
        break;
    case '&':
        token.type = TOKEN_CONJUNCTION;
        break;
    case '!':
        token.type = TOKEN_NEGATION;
        break;
    case '=':
        {
            bool eq = (it[1] == '=');
            it += eq;
            token.type = (eq ? TOKEN_EQUIVALENCE : TOKEN_EQUAL);
        }
        break;

    case '0':
    case '1':
        token.type = TOKEN_BOOL_LITERAL;
        break;
    default:
        if(IsAlpha(*it)) {
            while (IsAlpha(*it) || IsNumeric(*it) || *it == '_') it++;
            token.size = (it - tokenizer->begin) - token.begin;

            token.type = TOKEN_IDENTIFIER;
            for(auto i = keyWords; i < 1[&keyWords]; i++) {
                if(TokenEquals(tokenizer->begin, token, i->name)) {
                    token.type = i->type;
                }
            }

            tokenizer->at = it;
            return token;
        }
        else {
            while (IsAlpha(*it) || IsNumeric(*it) || *it == '_') it++;
            token.size = (it - tokenizer->begin) - token.begin;
            token.type = TOKEN_ERROR;
        }
        break;
    }
    
    tokenizer->at = ++it;
    token.size = (it - tokenizer->begin) - token.begin;
    return token;
}

u32 Tokenize(Tokenizer* tokenizer, Token* result) {

    u32 count = 0;

    for(auto t = GetToken(tokenizer); t.type != TOKEN_EOF; t = GetToken(tokenizer)) {
        ASSERT(t.type != TOKEN_ERROR);
        result[count++] = t;
    }
    return count;
}
bool TokensEqual(const char* text, Token t0, Token t1) {

    if(t0.size != t1.size) return false;
    auto b0 = text + t0.begin;
    auto b1 = text + t1.begin;

    for(u32 i = 0; i < t0.size; i++) {
        if(b0[i] != b1[i]) return false;
    }
    return true;
}

enum ExprType : u32 {
    EXPR_NONE,

    EXPR_LITERAL,
    EXPR_SYMBOL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_ASSIGNMENT,
    EXPR_CLEAR,

    EXPR_COUNT,
};

struct Expr {
    u32 index;
    u32 name;
};
struct LiteralExpr : Expr {
    bool c;
};
struct SymbolExpr : Expr {
    Token identifier;
};
struct UnaryExpr : Expr {
    u32 opr;
    Expr expr;
};
struct BinaryExpr : Expr {
    u32 opr;
    Expr left;
    Expr right;
};
struct ParsedResult {
    Expr expr;
    u32 consumedTokenCount;
    u32 consumedMemory;
    u32 name;
};
u32 Match(Token* token, const TokenType* types, u32 count) {
    for(u32 i = 0; i < count; i++) {
        if(token->type == types[i]) return i;
    }

    return ~u32(0);
}
bool GetBool(Tokenizer* tokenizer, Token t) {
    ASSERT(t.type == TOKEN_BOOL_LITERAL);
    return TokenEquals(tokenizer->begin, t, "true") || tokenizer->begin[t.begin] == '1';
}
bool ExpectToken(Token* stream, TokenType t) {
    auto cond = stream->type != t;
    if(cond) {
        global_print("%s%s%c", "Expected ", TOKEN_STRS[t], '\n');
    }
    return !cond;
}
ParsedResult ParsePrimary(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {

    TokenType types[] = {
        TOKEN_IDENTIFIER,
        TOKEN_NEGATION,
        TOKEN_OPEN_PAREN,
        TOKEN_BOOL_LITERAL,
    };

    auto opr = Match(stream, types, 4);
    switch(opr) {
    case 0:// identifier
        {
            ParsedResult ret;
            ret.expr.index = result - base;
            ret.consumedTokenCount = 1;
            ret.consumedMemory = sizeof(SymbolExpr);

            auto& symbol = Mem<SymbolExpr>(result);
            symbol.index = EXPR_SYMBOL;
            symbol.identifier = *stream;
            symbol.name = name + 1;
            ret.name = symbol.name;

            return ret;
        }
    case 1:// !expr
        {
            ParsedResult ret;
            ret.expr.index = result - base;
            auto& neg = Mem<UnaryExpr>(result);
            neg.opr = TOKEN_NEGATION;
            neg.index = EXPR_UNARY;

            result += sizeof(UnaryExpr);
            auto exprRes = ParsePrimary(stream+1, tokenizer, base ,result, name);
            neg.expr = exprRes.expr;

            neg.name = exprRes.name + 1;
            ret.consumedMemory = exprRes.consumedMemory + sizeof(UnaryExpr);
            ret.consumedTokenCount = exprRes.consumedTokenCount + 1;
            ret.name = neg.name;
            return ret;
        }
    case 2://(expr)
        {
            auto groupExpr = ParsePrimary(stream, tokenizer, base ,result, name);
            ExpectToken(stream + groupExpr.consumedTokenCount, TOKEN_CLOSE_PAREN);
            groupExpr.consumedTokenCount++;
            return groupExpr;
        }
    case 3:// literal
        {
            ParsedResult ret;
            ret.expr.index = result - base;
            ret.consumedMemory = sizeof(LiteralExpr);
            ret.consumedTokenCount = 1;

            auto& l = Mem<LiteralExpr>(result);
            l.c = GetBool(tokenizer, *stream);
            l.index = EXPR_LITERAL;
            l.name = name + 1;
            ret.name = l.name;

            return ret;
        }
    case ~u32(0):
        {
            ParsedResult ret;
            ret.expr.index = result - base;
            ret.consumedMemory = sizeof(Expr);
            ret.consumedTokenCount = 1;

            auto& l = Mem<Expr>(result);
            l.index = EXPR_NONE;
            l.name = name + 1;
            ret.name = l.name;

            return ret;
        }
    }
}
template<typename T, T ParseRight>
ParsedResult ParseHelper(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name, TokenType* tokens, u32 tokenCount, ParsedResult left) {

    for(u32 opr = Match(stream, tokens, tokenCount); opr != ~u32(0); opr = Match(stream, tokens, tokenCount)) {

        auto e = Expr{result - base};
        auto& bin = Mem<BinaryExpr>(result);

        bin.index = EXPR_BINARY;
        bin.left = left.expr;
        bin.opr = tokens[opr];

        stream++;
        result += sizeof(BinaryExpr);
        auto right = ParseRight(stream, tokenizer, base, result, left.name);

        bin.right = right.expr;
        stream += right.consumedTokenCount;
        result += right.consumedMemory;

        left.consumedMemory += right.consumedMemory + sizeof(BinaryExpr);
        left.consumedTokenCount += right.consumedTokenCount + 1;
        left.expr = e;
        bin.name = right.name + 1;
        left.name = bin.name;
    }

    return left;
}
ParsedResult ParseConjunction(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {
    
    auto left = ParsePrimary(stream, tokenizer, base, result, name);

    TokenType binaries[] = {
        TOKEN_CONJUNCTION,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    return ParseHelper<decltype(&ParsePrimary), ParsePrimary>(stream, tokenizer, base, result, name, binaries, 1, left);
}
ParsedResult ParseDisjunction(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {
    
    auto left = ParseConjunction(stream, tokenizer, base, result, name);

    TokenType binaries[] = {
        TOKEN_DISJUNCTION,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    return ParseHelper<decltype(&ParseConjunction), ParseConjunction>(stream, tokenizer, base, result, name, binaries, 1, left);
}
ParsedResult ParseImplication(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {

    auto left = ParseDisjunction(stream, tokenizer, base, result, name);

    TokenType binaries[] = {
        TOKEN_IMPLICATION,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    return ParseHelper<decltype(&ParseDisjunction), ParseDisjunction>(stream, tokenizer, base, result, name, binaries, 1, left);
}
ParsedResult ParseEquality(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {
    
    auto left = ParseImplication(stream, tokenizer, base, result, name);

    TokenType binaries[] = {
        TOKEN_EQUIVALENCE,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    return ParseHelper<decltype(&ParseImplication), ParseImplication>(stream, tokenizer, base, result, name, binaries, 1, left);
}
ParsedResult ParseAssignment(Token* stream, Tokenizer* tokenizer, byte* base, byte* result, u32 name) {

    auto left = ParseEquality(stream, tokenizer, base, result, name);

    TokenType binaries[] = {
        TOKEN_EQUAL,
    };

    result += left.consumedMemory;
    stream += left.consumedTokenCount;

    return ParseHelper<decltype(&ParseEquality), ParseEquality>(stream, tokenizer, base, result, name, binaries, 1, left);
}


u32 ParseTokenStream(Tokenizer* tokenizer, Token* stream, u32 tokenCount, byte* base, byte* result) {

    // max 100 expressions
    Expr* exprBuffer = (Expr*)result;
    result += sizeof(Expr) * 100;
    u32 count = 0;

    for(u32 i = 0; i < tokenCount;) {

        if(stream[i].type == TOKEN_KEYWORD_CLEAR) {
            Mem<ExprType>(result) = EXPR_CLEAR;
            i++;
            exprBuffer[count++].index = result - base;
            result += sizeof(ExprType);
            i += ExpectToken(stream + i, TOKEN_SEMICOLON);
            continue;
        }
        else if(stream[i].type == TOKEN_KEYWORD_EXIT) {

            Mem<ExprType>(result) = EXPR_NONE;
            i++;
            exprBuffer[count++].index = result - base;
            result += sizeof(ExprType);
            i += ExpectToken(stream + i, TOKEN_SEMICOLON);
            continue;
        }

        auto res = ParseAssignment(stream + i, tokenizer, base, result, 0);
        i += res.consumedTokenCount;
        result += res.consumedMemory;
        exprBuffer[count++] = res.expr;

        i += ExpectToken(stream + i, TOKEN_SEMICOLON);
    }

    return count;
}
void PrintExpr(Context* ctx, Expr expr) {

    auto t = Mem<ExprType>(ctx->exprBase + expr.index);
    switch(t) {
    case EXPR_LITERAL:
        {
            auto& e = Mem<LiteralExpr>(ctx->exprBase + expr.index);
            global_print("%i", e.c);
            break;
        }
    case EXPR_UNARY:
        {
            auto& e = Mem<UnaryExpr>(ctx->exprBase + expr.index);
            ASSERT(e.opr == TOKEN_NEGATION);
            global_print("%c", '!');
            PrintExpr(ctx, e.expr);
            break;
        }
    case EXPR_BINARY:
        {
            auto& e = Mem<BinaryExpr>(ctx->exprBase + expr.index);
            global_print("%c", '(');
            PrintExpr(ctx, e.left);
            global_print("%s", TOKEN_STRS[e.opr]);
            PrintExpr(ctx, e.right);
            global_print("%c", ')');
            break;
        }
    case EXPR_SYMBOL:
        {
            auto& e = Mem<SymbolExpr>(ctx->exprBase + expr.index);
            auto str = ctx->tokenizer.begin + e.identifier.begin;
            global_print("%s*", str, e.identifier.size);
            break;
        }
    default:ASSERT(false);break;
    }
}


u32 GetSymbolsFromExpr(Context* ctx, Expr expr, byte* mem) {
    
    auto t = Mem<ExprType>(ctx->exprBase + expr.index);
    switch(t) {
    case EXPR_LITERAL:
        return 0;
    case EXPR_UNARY:
        return GetSymbolsFromExpr(ctx, Mem<UnaryExpr>(ctx->exprBase + expr.index).expr, mem);
    case EXPR_BINARY:
        {
            auto& e = Mem<BinaryExpr>(ctx->exprBase + expr.index);
            u32 ret = 0;
            ret += GetSymbolsFromExpr(ctx, e.left, mem);
            return ret + GetSymbolsFromExpr(ctx, e.right, mem + ret * sizeof(Token));
        }
    case EXPR_SYMBOL:
        {
            auto& e = Mem<SymbolExpr>(ctx->exprBase + expr.index);
            Mem<Token>(mem) = e.identifier;
            return 1;
        }
    }
}
u32 SizeofExpr(Context* ctx, Expr expr) {

    auto t = Mem<ExprType>(ctx->exprBase + expr.index);
    switch(t) {
    case EXPR_LITERAL:
    case EXPR_SYMBOL:
        return 1;
    case EXPR_UNARY:
        return SizeofExpr(ctx, Mem<UnaryExpr>(ctx->exprBase + expr.index).expr) + 1;
    case EXPR_BINARY:
        {
            auto& e = Mem<BinaryExpr>(ctx->exprBase + expr.index);
            return SizeofExpr(ctx, e.left) + SizeofExpr(ctx, e.right) + 1;
        }
    }
}


struct Interpretation {
    u32 count;
    bool values[];
};
u32 GetExprIndex(Expr* exprs, u32 count, Expr e) {
    for(u32 i = 0; i < count; i++) {
        if(exprs[i].index == e.index) return i;
    }
    return ~u32(0);
}

bool EvalOpr(u32 opr, bool l, bool r) {
    switch(opr) {
    case TOKEN_NEGATION:
        return !l;
    case TOKEN_CONJUNCTION:
        return l & r;
    case TOKEN_DISJUNCTION:
        return l | r;
    case TOKEN_IMPLICATION:
        return (!l) | r;
    case TOKEN_EQUIVALENCE:
        return l == r;
    }
    ASSERT(false);
}

Token GetNameExpr(Context* ctx, Expr e) {

    auto exp = (Expr*)(ctx->exprBase + e.index);
    switch(exp->index) {
    case EXPR_NONE: 
    case EXPR_LITERAL:
    case EXPR_BINARY:
        ASSERT(false);
        break;
    case EXPR_ASSIGNMENT:
        {
            auto& assignment = Mem<BinaryExpr>(exp);
            return GetNameExpr(ctx, assignment.left);
            break;
        }
    case EXPR_SYMBOL:
        return Mem<SymbolExpr>(exp).identifier;
    }
    ASSERT(false);
}
bool EvalExpr(Context* ctx, Expr e) {

    auto exp = (Expr*)(ctx->exprBase + e.index);
    
    switch(exp->index) {
    case EXPR_NONE: ASSERT(false);
    case EXPR_LITERAL:
        return Mem<LiteralExpr>(exp).c;
    case EXPR_BINARY:
        {
            auto& bin = Mem<BinaryExpr>(exp);
            bool l = EvalExpr(ctx, bin.left);
            bool r = EvalExpr(ctx, bin.right);
            return EvalOpr(bin.opr, l,r);
        }
    case EXPR_ASSIGNMENT:
        {
            auto& assignment = Mem<BinaryExpr>(exp);
            auto name = GetNameExpr(ctx, assignment.left);
            auto right = EvalExpr(ctx, assignment.right);
            for(u32 i = 0; i < ctx->environmentSize; i++) {
                if(TokensEqual(ctx->tokenizer.begin, ctx->environment[i].name, name)) {
                    ctx->environment[i].value = right;
                    return right;
                }
            }

            u32 index = ctx->environmentSize++;
            ASSERT(index < ctx->environmentCap);
            ctx->environment[index].name = name;
            ctx->environment[index].value = right;
            return right;
        }
    case EXPR_SYMBOL:
        {
            auto& s = Mem<SymbolExpr>(exp);

            for(u32 i = 0; i < ctx->environmentSize; i++) {
                if(TokensEqual(ctx->tokenizer.begin, ctx->environment[i].name, s.identifier)) {
                    return ctx->environment[i].value;
                }
            }
            u32 index = ctx->environmentSize++;
            ASSERT(index < ctx->environmentCap);
            ctx->environment[index].name = s.identifier;
            return ctx->environment[index].value;
        }
    }
    ASSERT(false);
}
u32 GetSymbols(Context* ctx, Expr e, u32* mem) {
    
    auto t = Mem<ExprType>(ctx->exprBase + e.index);
    switch(t) {
    case EXPR_LITERAL:
    case EXPR_CLEAR:
        return 0;
    case EXPR_SYMBOL:
        *mem = e.index;
        return 1;
    case EXPR_UNARY:
        return GetSymbols(ctx, Mem<UnaryExpr>(ctx->exprBase + e.index).expr, mem);
    case EXPR_ASSIGNMENT:
    case EXPR_BINARY:
        {
            u32 count = GetSymbols(ctx, Mem<BinaryExpr>(ctx->exprBase + e.index).left, mem);
            count += GetSymbols(ctx, Mem<BinaryExpr>(ctx->exprBase + e.index).right, mem + count);
            return count;
        }
    }
    ASSERT(false);
}

bool MakeInterpretation(Context* ctx, Expr e, bool* table) {

    auto t = Mem<Expr>(ctx->exprBase + e.index).index;
    auto i = Mem<Expr>(ctx->exprBase + e.index).name;
    switch(t) {
    case EXPR_LITERAL:
        table[i] = Mem<LiteralExpr>(ctx->exprBase + e.index).c;
        return table[i];
    case EXPR_CLEAR:
        ASSERT(false);
    case EXPR_SYMBOL:
        return table[i];
    case EXPR_UNARY:
        {
            auto& u = Mem<UnaryExpr>(ctx->exprBase + e.index);
            auto intpret = MakeInterpretation(ctx, u.expr, table);
            table[i] = EvalOpr(u.opr, intpret, 0);
            return table[i];
        }
    case EXPR_BINARY:
        {
            auto& b = Mem<BinaryExpr>(ctx->exprBase + e.index);
            bool left = MakeInterpretation(ctx, b.left, table);
            bool right = MakeInterpretation(ctx, b.right, table);
            table[i] = EvalOpr(b.opr, left, right);
            return table[i];
        }
    case EXPR_ASSIGNMENT:
        {
            auto& b = Mem<BinaryExpr>(ctx->exprBase + e.index);
            bool right = MakeInterpretation(ctx, b.right, table);
            table[Mem<Expr>(ctx->exprBase + b.right.index).name] = right;
            table[i] = right;
            return right;
        }
    }
    ASSERT(false);
}

struct TruthTable {
    u32 rowCount;
    u32 collumnCount;
    bool table[];
};
u32 UnifySymbols(Context* ctx, Expr e, u32* mem) {
    auto t = Mem<ExprType>(ctx->exprBase + e.index);
    switch(t) {
    case EXPR_LITERAL:
    case EXPR_CLEAR:
        Mem<Expr>(ctx->exprBase + e.index).name = ctx->exprCount++;
        return e.index;
    case EXPR_SYMBOL:
        {
            auto& s = Mem<SymbolExpr>(ctx->exprBase + e.index);
            for(u32 i = 1; i < mem[0] + 1; i++) {
                auto it = Mem<SymbolExpr>(ctx->exprBase + mem[i]).identifier;
                if(TokensEqual(ctx->tokenizer.begin, s.identifier, it)) {
                    return mem[i];
                }
            }
            s.name = ctx->exprCount++;
            mem[++mem[0]] = e.index;
            return e.index;
        }
    case EXPR_UNARY:
        {
            auto& u = Mem<UnaryExpr>(ctx->exprBase + e.index);
            u.expr.index = UnifySymbols(ctx, u.expr, mem);
            u.name = ctx->exprCount++;
            return e.index;
        }
    case EXPR_ASSIGNMENT:
    case EXPR_BINARY:
        {
            auto& bin = Mem<BinaryExpr>(ctx->exprBase + e.index);
            bin.left.index = UnifySymbols(ctx, bin.left, mem);
            bin.right.index = UnifySymbols(ctx, bin.right, mem);
            bin.name = ctx->exprCount++;
            return e.index;
        }
    }
    ASSERT(false);
}
void MakeTruthTable(Context* ctx, Expr e, TruthTable* truthTable) {

    {
        Mem<u32>(truthTable) = 0;
        ctx->exprCount = 0;
        e.index = UnifySymbols(ctx, e, (u32*)truthTable);
    }

    auto symbolCount = Mem<u32>(truthTable);
    u32 symbols[symbolCount];
    memcpy(symbols, ((u32*)truthTable) + 1, symbolCount * sizeof(u32));

    auto exprCount = ctx->exprCount + 1;

    truthTable->rowCount = exprCount;
    truthTable->collumnCount = i64_power(2, symbolCount);

    bool localMem[symbolCount * 2]{};
    bool* symbolState = localMem;
    bool* symbolState2 = localMem + symbolCount;

    for(u32 i = 0; i < i64_power(2, symbolCount); i++) {

        for(u32 k = 0; k < symbolCount; k++) {
            auto name = Mem<Expr>(ctx->exprBase + symbols[k]).name;
            truthTable->table[i * exprCount + name] = symbolState[k];
        }
        IncBinary(symbolState, symbolCount, symbolState2);
        auto tmp = symbolState;
        symbolState = symbolState2;
        symbolState2 = tmp;

        MakeInterpretation(ctx, e, truthTable->table + i * exprCount);
    }

}

Context MakeReplContext(byte* mem) {
    
    Context ctx;
    ctx.name = "repl";
    ctx.tokenizer.begin = (char*)mem;
    ctx.tokenizer.end = (char*)mem + Kilobyte(8);
    ctx.tokenStream = (Token*)(mem + Kilobyte(8));
    ctx.exprBase = mem + Kilobyte(16);
    ctx.environment = (Symbol*)(ctx.exprBase + Kilobyte(8));
    ctx.environmentCap = 150;
    ctx.environmentSize = 0;
    byte* exprMem = ctx.exprBase;
    ctx.tokenizer.at = ctx.tokenizer.begin;
    
    return ctx;
}
u32 ParseArgs(u32 argc, const char* argv[], Context* result, byte* mem) {

    u32 ret = 0;
    for(u32 i = 1; i < argc; i++) {
        auto path = argv[i];
        if(str_cmp(path, "-repl")) {
            Mem<Context>(result + ret++) = MakeReplContext(mem);
            continue;
        }
        u32 descriptor = open(path, O_RDONLY);
        if(descriptor == ~u32(0)) {
            global_print("%s%s%c", "Errors opening file: ", path, '\n');
            continue;
        }

        auto& ctx = Mem<Context>(result + ret++);
        ctx.name = path;
        ctx.tokenizer.begin = (char*)mem;
        ctx.tokenizer.end = (char*)mem + Kilobyte(8);
        ctx.tokenStream = (Token*)(mem + Kilobyte(8));
        ctx.exprBase = mem + Kilobyte(16);
        ctx.environment = (Symbol*)(ctx.exprBase + Kilobyte(8));
        ctx.environmentCap = 150;
        ctx.environmentSize = 0;
        byte* exprMem = ctx.exprBase;
        ctx.tokenizer.at = ctx.tokenizer.begin;
        auto readBytes = read(descriptor, (void*)ctx.tokenizer.at, Kilobyte(8)-1);
        Mem<char>((char*)(ctx.tokenizer.at + readBytes)) = 0;

        mem += Kilobyte(27) + 600;
    }
    return ret;
}

void PrintIntpret(Context* ctx, Expr expr, bool* intpret) {

    auto t = Mem<ExprType>(ctx->exprBase + expr.index);
    switch(t) {
    case EXPR_LITERAL:
        {
            auto& e = Mem<LiteralExpr>(ctx->exprBase + expr.index);
            global_print("%i", e.c);
            break;
        }
    case EXPR_UNARY:
        {
            auto& e = Mem<UnaryExpr>(ctx->exprBase + expr.index);
            global_print("%i%c", (i64)intpret[e.name], ' ');
            PrintIntpret(ctx, e.expr, intpret);
            break;
        }
    case EXPR_BINARY:
        {
            auto& e = Mem<BinaryExpr>(ctx->exprBase + expr.index);
            global_print("%c", '(');
            PrintIntpret(ctx, e.left, intpret);
            global_print("%i%c", (i64)intpret[e.name], ' ');
            PrintIntpret(ctx, e.right, intpret);
            global_print("%c", ')');
            break;
        }
    case EXPR_SYMBOL:
        {
            auto& e = Mem<SymbolExpr>(ctx->exprBase + expr.index);
            global_print("%i%c", (i64)intpret[e.name], ' ');
            break;
        }
    default:ASSERT(false);break;
    }
}
bool IsFormulaLaw(Context* ctx, Expr e, TruthTable* table) {

    auto name = Mem<Expr>(ctx->exprBase + e.index).name;
    bool law = true;
    for(u32 i = 0; i < table->collumnCount; i++) {
        law &= table->table[i * table->rowCount + name];
    }
    return law;
}
bool IsFormulaContradiction(Context* ctx, Expr e, TruthTable* table) {

    auto name = Mem<Expr>(ctx->exprBase + e.index).name;
    bool contradiction = true;
    for(u32 i = 0; i < table->collumnCount; i++) {
        contradiction &= !table->table[i * table->rowCount + name];
    }
    return contradiction;
}
bool IsFormulaSatisfiable(Context* ctx, Expr e, TruthTable* table) {

    auto name = Mem<Expr>(ctx->exprBase + e.index).name;
    for(u32 i = 0; i < table->collumnCount; i++) {
        if(table->table[i * table->rowCount + name]) return true;
    }
    return false;
}
void RunReplContext(Context* ctx) {

    global_print("%s", "repl mode\ntype \"exit\" to leave");

    byte localMem[Kilobyte(8)];

    bool running = true;
    for(;running;) {

        global_print("%s", "\n>");
        global_io_flush();
        auto readByteCount = read(STDIN_FILENO, (void*)(ctx->tokenizer.at), Kilobyte(8));
        ((char*)(ctx->tokenizer.at))[readByteCount] = 0;

        auto count = Tokenize(&ctx->tokenizer, ctx->tokenStream);
        auto exprCount = ParseTokenStream(&ctx->tokenizer, ctx->tokenStream, count, ctx->exprBase, ctx->exprBase);
        ctx->tokenStream += count;

        for(u32 i = 0; i < exprCount; i++) {
            auto e = ((Expr*)ctx->exprBase)[i];
            auto t = Mem<ExprType>(ctx->exprBase + e.index);
            if(t == EXPR_CLEAR) {
                system("clear");
                continue;
            }
            else if(t == EXPR_NONE) {
                running = false;
                break;
            }
            auto table = (TruthTable*)localMem;
            MakeTruthTable(ctx, e, table);
            for(u32 k = 0; k < table->collumnCount; k++) {
                PrintIntpret(ctx, e, table->table + table->rowCount * k);
                global_print("%c", '\n');
            }
            if(IsFormulaLaw(ctx, e, table)) {
                global_print("%s", "Law\n");
            }
            if(IsFormulaSatisfiable(ctx, e, table)) {
                global_print("%s", "Satisfiable\n");
            }
            if(IsFormulaContradiction(ctx, e, table)) {
                global_print("%s", "Contradiction\n");
            }
        }
    }
}

ContextStatus RunContext(Context ctx) {

    global_print("%s%s%c", "Running ", ctx.name, '\n');
    auto count = Tokenize(&ctx.tokenizer, ctx.tokenStream);
    auto exprCount = ParseTokenStream(&ctx.tokenizer, ctx.tokenStream, count, ctx.exprBase, ctx.exprBase);

    for(u32 i = 0; i < exprCount; i++) {
        auto e = ((Expr*)ctx.exprBase)[i];
        auto t = Mem<ExprType>(ctx.exprBase + e.index);
        if(t == EXPR_CLEAR) {
            system("clear");
            continue;
        }
        else if(t == EXPR_NONE) {
            return CTX_EXIT;
        }

        auto v = EvalExpr(&ctx, e);
        global_print("%f%s", v, ", ");
    }

    return CTX_EOF;
}

i32 main(u32 argc, const char* argv[]) {
   
    auto scratch = init_global_state(0, argc * (Kilobyte(27) + 600), 512);
    {
        Context ctxs[argc-1];
        u32 ctxCount = ParseArgs(argc, argv, ctxs, scratch);
        for(u32 i = 0; i < ctxCount; i++) {

            if(str_cmp(ctxs[i].name, "repl")) {
                RunReplContext(ctxs + i);
            }
            else {
                auto res = RunContext(ctxs[i]);
                global_print("%s%s", "\ncontext ", res == CTX_EOF ? "eof\n" : "exited\n");
            }
            global_print("%c", '\n');
        }
    }

    global_io_flush();

    return 0;
}