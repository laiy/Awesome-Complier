## AQL(Annotation Query Language) Subset Compiler

### What Is The Subset Mean?
This project is not a complete Compiler of AQL, but it realizes the major part of it.

### Why Realize A Exited Language Compiler?
This compiler project is regarded as a nice teaching example of a compiler when studying Compilers.

### Language

> The subset language of AQL, containing the create and output gramma.

ql_stmt → create_stmt ; | output_stmt ;  
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

