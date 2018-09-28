#pragma once

#define SELECT_QUESTION_ID(SQL, CONDITION) \
 sprintf(SQL, "select F_ID from T_QA_LIST where F_TYPE = 'question' and (%s)", CONDITION.c_str())

#define SELECT_QUESTION_ID_RANGE_ID(SQL, CONDITION, IDLIST) \
  sprintf(SQL, "select F_ID from T_QA_LIST where F_ID in (%s) and (%s) ", IDLIST.c_str(), CONDITION.c_str())


//�õ���id
#define SELECT_GROUP_ID(SQL, QUESTIONID) \
  sprintf(SQL,"select F_GROUP_ID from T_QA_LIST where F_ID = %d", QUESTIONID)

//�õ������ز���
#define SELECT_GROUP_PARAM(SQL, GROUPID) \
  sprintf(SQL, "select a.F_AUDIO, a.F_IS_END, a.F_IS_BREAK, b.F_TYPE, b.F_LOOP, b.F_AFFIRM from T_QA_GROUP_LIST a, T_QA_CLASSIFY_LIST b where a.F_GROUP_ID = %d and a.F_CLASSIFY_ID = b.F_ID", GROUPID)

//�õ�һ�����һ�����answer
#define SELECT_ANSWER_BY_GROUPID(SQL, GROUPID) \
	sprintf(SQL, "select * from T_QA_LIST where F_GROUP_ID = %d and F_TYPE = 'answer'order by random() limit 1", GROUPID)

//�õ������ַ�����questionId
#define SELECT_QUESTION_STR(SQL, QUESTIONID) \
    sprintf(SQL, "select * from T_QA_LIST where F_ID = %d", QUESTIONID);


//�õ�һ���������question
#define SELECT_ALLQUESTION_BY_GROUPID(SQL, GROUPID) \
   sprintf(SQL, "select * from T_QA_LIST where F_GROUP_ID = %d and F_TYPE = 'question'",GROUPID)

//�õ�ÿ�������Q��ļ�¼, ��Q�ﲻ����@
#define SELECT_QUESTION_ID_BY_GROUP(SQL, GROUPID) \
	sprintf(SQL, "select F_ID from T_QA_LIST where F_GROUP_ID = %d and F_TYPE = 'question' and F_SENTENCE not like ('%%@%%') order by length(F_SENTENCE) desc limit 1", GROUPID)

//�õ���ȫƥ���questionID
#define SELECT_QUESTION_MATCH(SQL, QUESTION) \
    sprintf(SQL, "select a.F_ID from T_QA_LIST a, T_QA_GROUP_LIST b where a.F_SENTENCE = '%s' and a.F_TYPE = 'question' and b.F_GROUP_ID = a. F_GROUP_ID order by a.F_ID limit 1", QUESTION.c_str())
///////////////////////////////////////////////////////////////////////////////////////////////
#define SELECT_EN2CN_FIELD(SQLSTRING, TYPE) \
		sprintf(SQLSTRING, "select F_TYPE, F_EN, F_CN from T_ENTOCN where F_TYPE = %d", TYPE)

//////////////////////////////////////////////////////////////////////////////////////////////
//�;����ȶ���ص�
#define SELECT_COMPANY_QUESTION(SQLSTRING, QUESTIONID, GROUPID ) \
	sprintf(SQLSTRING, "select F_FREQUENCY from T_COMPANY_QUESTION_FREQUENCY where F_ID = %d and F_GROUP_ID = %d", QUESTIONID, GROUPID)

#define SELECT_COMPANY_QUESTIONID_COUNT(SQLSTRING, QUESTIONID, GROUPID) \
	sprintf(SQLSTRING, "select count(*) as F_MAXCOUNT from T_COMPANY_QUESTION_FREQUENCY where F_ID = %d and F_GROUP_ID = %d ", QUESTIONID, GROUPID)
	
#define INSERT_COMPANY_FREQUENCY(SQLSTRING,  QUESTIONID, GROUPID, FREQUENCY) \
    sprintf(SQLSTRING, "insert into T_COMPANY_QUESTION_FREQUENCY(F_ID, F_GROUP_ID, F_FREQUENCY) values(%d, %d, %d)", QUESTIONID, GROUPID, FREQUENCY)

#define UPDATE_COMPANY_FREQUENCY(SQLSTRING,  QUESTIONID, GROUPID) \
	sprintf(SQLSTRING, "update T_COMPANY_QUESTION_FREQUENCY set F_FREQUENCY = F_FREQUENCY +1  where F_ID = %d and F_GROUP_ID = %d", QUESTIONID, GROUPID)

/////////////////////////////////////////////////////////////////////////////////////////////////
//���׾��ӵ�ѡ��
#define SELECT_DEFAULT_ANSWER(SQLSTRING, TYPE) \
   sprintf(SQLSTRING, "select F_SENTENCE from T_DEFAULT_ANSWER where F_TYPE = %d order by random() limit 1", TYPE)

//��ôʴ�
#define SELECT_PRIMARY_WORD(SQLSTRING, INWORD) \
   sprintf(SQLSTRING, "select F_ID from T_PRIMARY_WORD where F_WORD like '%%%s%%'", INWORD.c_str())
//������еĹؼ���
#define SELECT_ALL_PRIMARY_WORD(SQLSTRING) \
	sprintf(SQLSTRING, "select * from t_special_words order by length(f_word) desc")
/////////////////////////////////////////////////////////////////////////////////////////////////
 
//�õ���ǰ����͵ڼ���
#define GET_YEAR_WEEK(SQLSTRING) \
	sprintf(SQLSTRING, "select strftime('%%Y','now') as YEAR, strftime('%%W', 'now') as WEEK ")

//�õ�ְԱָ��ʱ����Ű��
#define GET_EMPLOYEES_SCHEDULE(SQLSTRING, EMPLOYEESID, YEAR, WEEK) \
	sprintf(SQLSTRING, "select * from T_COMPANY_EMPLOYEES_SCHEDULE where F_EMPLOYEES_ID = %d and F_YEAR = %d and F_WEEK = %d", EMPLOYEESID, YEAR, WEEK)

//�õ�����ָ��ʱ����Ű��
#define GET_DEPARTMENT_SCHEDULE(SQLSTRING, DEPARTMENTID, YEAR, WEEK) \
		sprintf(SQLSTRING, "select * from T_COMPANY_DEPARTMENT_SCHEDULE where F_DEPARTMENT_ID = %d and F_YEAR = %d and F_WEEK = %d", DEPARTMENTID, YEAR, WEEK)

///////////////////////////////////////////////////////////////////////////////////////////////////
//�õ���̬�ı�
#define GET_STATIC_QUESTION_GROUPID(SQLSTRING, QUESTION) \
sprintf(SQLSTRING, "select F_GROUP_ID from T_QA_LIST where F_TYPE = 'question' and F_SENTENCE = '%s'", QUESTION.c_str())

#define GET_STATI_ANSWER(SQLSTRING, GROUPID) \
sprintf(SQLSTRING, "select F_SENTENCE from T_QA_LIST where F_GROUP_ID = %d and F_TYPE = 'answer' limit 1", GROUPID)

///////////////////////////////////////////////////////////////////////////////////////////////////
//��Ҷ˹����
#define GET_BAYES_DATA(SQLSTRING, MODULE) sprintf(SQLSTRING, "select * from T_BAYES_TRAINED_DATA where F_MODULE = '%s'", MODULE.c_str())

