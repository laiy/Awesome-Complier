## AQL(Annotation Query Language) Subset Compiler

### What Is The Subset Mean?
This project is not a complete Compiler of AQL, but it realizes the major part of it.

### Why Realize A Exited Language Compiler?
This compiler project is regarded as a nice teaching example of a compiler when studying Compilers.

### Language

> The subset language of AQL, containing the create and output gramma.

aql_stmt → create_stmt ; | output_stmt ;  
create_stmt → create view ID as view_stmt  
view_stmt → select_stmt | extract_stmt  
output_stmt → output view ID alias  
alias → as ID | ε  
elect_stmt → select select_list from from_list  
select_list → select_item | select_list , select_item  
select_item → ID . ID alias  
from_list → from_item | from_list , from_item  
from_item → ID ID  
extract_stmt → extract extract_spec from from_list  
extract_spec → regex_spec | pattern_spec  
regex_spec → regex REG on column name_spec  
column → ID . ID  
name_spec → as ID | return group_spec  
group_spec → single_group | group_spec and single_group  
single_group → group NUM as ID  
pattern_spec → pattern pattern_expr name_spec  
pattern_expr → pattern_pkg | pattern_expr pattern_pkg  
pattern_pkg → atom | atom { NUM , NUM } | pattern_group  
atom→ < column > | < Token > | REG  
pattern_group → ( pattern_expr )  

### Lexer
Scan the original language and output the terminator to gramma parser.

### Parser
Create a gramma tree and execute the program.

### Tokenizer
When executing the program, we need to get the tokens of the document to be executed, it would help us to divide token from the original document.

### 吐槽
能不能不要为了为难学生而为难学生？  
全部写完了才给个附加要求，而且一些要求在我看来根本就是为了把问题搞复杂而做的，而不是在“补充”。  
像第一个要求，REG已经是终结符了，最早的说法很清楚对于group的捕获来自于Language pattern_group -> ( pattern_expr ) 这里的"("和")"终结符。  
现在竟然让REG终结符里面的内容还需要捕获括号在语法上真的说不过去，这是语法的东西吗？换句话说，REG在文法中不可见的括号竟然突然被加到Language里去了。  
这合理吗？如果这个涉及文法不是应该在Language里面给出吗？  
而在3.1中又忽略REG内部括号的捕获了，不觉得自相矛盾吗？  
REGEX在我做的时候的理解就是语义上只能返回一个group，语法上可以多个，但是有多个group直接throw一个语义错误不是比较合理吗。  
group就是写在文法里面的东西，它的执行我认为就应该直接和最终的抽象语法树直接挂钩，而不是在终结符里面还去挑括号来匹配。  
把语法树可见的终结符和终结符REG中语法树不可见的东西来执行同一个编译器后端逻辑下不觉得整个逻辑很有问题吗？  

### 没有实现的地方
附加要求1。  
附加要求3.2, 只支持REG, Token{min, max}， COLUMN三种模式的匹配。  

### 附加要求5运行环境
ubuntu14.04

