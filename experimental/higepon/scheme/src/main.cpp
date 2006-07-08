/*!
  \file   main.cpp
  \brief  scheme

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/06/16 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "scheme.h"
#include "parser.h"

using namespace monash;
using namespace std;


Object* evalSequence(Objects* exps, Environment* env)
{
    Object* ret = NULL;
    for (Objects::iterator it = exps->begin(); it != exps->end(); it++)
    {
        ret = eval(*it, env);
    }
    return ret;
}

bool isCompoundProcedure(Object* p)
{
    return p->type() == Object::PROCEDURE;
}

Object* apply(Object* procedure, Objects* arguments)
{
    // fix me
    // if primitive procedure
    if (isCompoundProcedure(procedure))
    {
        Procedure* p = (Procedure*)procedure;
        p->env()->extend(p->parameters(), arguments); // doubt? we need copy?
        evalSequence(p->body(), p->env());
    }

}

bool isTrue(Object* exp)
{
    if (exp->type() == Object::NUMBER)
    {
        Number* n = (Number*)exp;
        if (n->value() == 0) return false;
    }
    return true;
}


void display(Object* exp)
{
    printf("%s", exp->toString().c_str());
}

bool isSelfEvaluating(Object* exp)
{
    if (exp->type() == Object::NUMBER || exp->type() == Object::STRING)
    {
        return true;
    }
    return false;
}

bool is_variable(Object* exp)
{
    return (exp->type() == Object::VARIABLE);
}

bool is_quated(Object* exp)
{
    return (exp->type() == Object::QUOTE);
}

bool is_assignment(Object* exp)
{
    return (exp->type() == Object::ASSIGNMENT);
}

bool is_definition(Object* exp)
{
    return  (exp->type() == Object::DEFINITION);
}

bool isIf(Object* exp)
{
    return  (exp->type() == Object::IF);
}

bool is_lambda(Object* exp)
{
    return (exp->type() == Object::LAMBDA);
}

bool is_begin(Object* exp)
{
    return (exp->type() == Object::BEGIN);
}

bool is_cond(Object* exp)
{
    return false;
}

bool is_application(Object* exp)
{
    return false;
}

Object* text_of_quotation(Object* exp)
{
    return exp; // different from SICP's
}

Object* eval_definition(Object* exp, Environment* env)
{
    Assignment* assignment = (Assignment*)exp;
    env->defineVariable(assignment->variable(), assignment->value());
    return new Quote("OK"); // different from SICP's
}

Object* eval(Object* exp, Environment* env)
{
    if (isSelfEvaluating(exp))
    {
        return exp;
    }
    else if (is_variable(exp))
    {
        return env->lookupVariableValue((Variable*)exp);
    }
    else if (is_quated(exp))
    {
        return text_of_quotation(exp);
    }
    else if (is_assignment(exp))
    {
        Assignment* assignment = (Assignment*)exp;
        return assignment->eval(env);
    }
    else if (is_definition(exp))
    {
        Definition* definition = (Definition*)exp;
        return definition->eval(env);
    }
    else if (isIf(exp))
    {
        return SpecialIf::eval((SpecialIf*)exp, env);
    }
    else if (is_lambda(exp))
    {
        Lambda* lambda = (Lambda*)exp;
        Procedure* procedure = new Procedure(lambda, env);
        return (Object*)procedure;
    }
    else if (is_begin(exp))
    {
        Begin* begin = (Begin*)exp;
        return evalSequence(begin->actions(), env);
    }
    else if (is_cond(exp))
    {
    }
    // quick hack fix me
    else if (exp->type() == Object::PROCEDURE)
    {
        Procedure* procedure = (Procedure*)exp;
        return evalSequence(procedure->body(), procedure->env());
    }
    else if (is_application(exp))
    {
    }
    else
    {
        fprintf(stderr, "Unknown expression type -- EVAL %s ", exp);
    }
    return 0;
}

#include <string>

void printNodes(Node* node, int depth = 0);
void printNodes(Node* node, int depth)
{
    printf("\n");
    for (int i = 0; i < depth; i++) printf(" ");

    if (NULL == node)
    {
        printf("NULL");
        return;
    }
    printNode(node);
//    printf("[left:");
    depth++;
    printNodes(node->left, depth);
//    printf("]");
//    printf("[right:");
    printNodes(node->right, depth);
//    printf("]");
}

int main(int argc, char *argv[])
{
    Node* node = parseLeft();
//    printNodes(parseLeft());
    if (node->nodetype != Node::FUNCTION_CALL)
    {
        fprintf(stderr, "invalid application\n");
    }

    if (node->type == IDENTIFIER)
    {
        if (node == "define")
        {

        }

    }


    return 0;
    for (;;)
    {
        toknize();

    }
    return 0;
#if 0
    Number a(0);
    Number b(1);
    String c("hoge");
    SpecialIf exp(&a, &b, &c);
    display(eval(&exp, NULL));
#endif
    String str("hige");
    Objects body;
    body.push_back(&str);
    Variables parameters;
    parameters.push_back(new Variable(std::string("val")));
    Lambda lambda(&body, &parameters);
    Environment* env = new Environment();
    Object* o = eval(&lambda, env);
    printf("%s %s:%d [%s]\n", __func__, __FILE__, __LINE__, o->toString().c_str());fflush(stdout);
    Object* c = eval(o, env);
    printf("%s %s:%d [%s]\n", __func__, __FILE__, __LINE__, c->toString().c_str());fflush(stdout);
    //
//    display(eval, env));
    return 0;
}