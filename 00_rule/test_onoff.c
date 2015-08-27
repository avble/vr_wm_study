#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "vtimer.h"
#include "onoff.h"



int main(int argc, char *argv[])
{

    // test1: add 2 timers 
    vrule_init();
    vScheduler_init();
    VRuleInfo *p_rule_info = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));
    VRuleInfo *p_rule_info1 = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));
    VRuleInfo *p_rule_info2 = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));
    VRuleInfo *p_rule_info3 = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));
    VRuleInfo *p_rule_info4 = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));
    VRuleInfo *p_rule_info5 = (VRuleInfo *)calloc(1, sizeof(VRuleInfo));

    p_rule_info->ruleType = e_TIMER_RULE;
    p_rule_info->time = time(0) + 5;
    strcpy(p_rule_info->ruleName, "rule1");

    p_rule_info1->ruleType = e_TIMER_RULE;
    p_rule_info1->time = time(0) + 10;
    strcpy(p_rule_info1->ruleName, "rule2");

    p_rule_info2->ruleType = e_TIMER_RULE;
    p_rule_info2->time = time(0) + 15;
    strcpy(p_rule_info2->ruleName, "rule3");

    p_rule_info3->ruleType = e_TIMER_RULE;
    p_rule_info3->time = time(0) + 15;
    strcpy(p_rule_info3->ruleName, "rule4");

    p_rule_info4->ruleType = e_TIMER_RULE;
    p_rule_info4->time = time(0) + 15;
    strcpy(p_rule_info4->ruleName, "rule5");

    p_rule_info5->ruleType = e_TIMER_RULE;
    p_rule_info5->time = time(0) + 15;
    strcpy(p_rule_info5->ruleName, "rule6");


    //printf("%s %d \n", __func__, __LINE__);
    
    usleep(100*1000);
    
    //printf("%s %d \n", __func__, __LINE__);
    
    vrule_add(p_rule_info);
    //printf("%s %d \n", __func__, __LINE__);
    
//    usleep(100*1000);
    vrule_add(p_rule_info1);
    //printf("%s %d \n", __func__, __LINE__);
    
 //   usleep(100*1000);
    vrule_add(p_rule_info2);
    //printf("%s %d \n", __func__, __LINE__);
    
    vrule_add(p_rule_info3);
    //printf("%s %d \n", __func__, __LINE__);

    vrule_add(p_rule_info4);
    //printf("%s %d \n", __func__, __LINE__);

    vrule_add(p_rule_info5);
    //printf("%s %d \n", __func__, __LINE__);

    //vrule_print();
    
    usleep(30*1000*1000);

    
    // add 
}
