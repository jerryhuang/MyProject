/*
#  Copyright (C) 2011, Intel Corporation
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation; either version 2 of the License,
#  or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA.
#
#   Authors:
#           
*/
//#include "helper.h"
//#include "mysql_db.h"
#include "service.h"

#include <vector>
#include <string>
#include <map>
//#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

int vector_copy(const vector<string> input, vector<string> & output)
{
	output.clear();
	for(size_t i=0;i<input.size();i++)
	{
		output.push_back(input[i]);
	}
	return output.size();
}

int paser_getbuildid(vector<string> input, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		size_t pos_dot = input[i].find_first_of(".");
		output.push_back("0806.2");
		output.push_back("0816.2");
		output.push_back("0814.2");
		output.push_back("0805.2");
		//output.push_back(input[i]);
	}
	return output.size();
}

int paser_like(vector<string> input, vector<string> keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		for(size_t j=0;j<keyword.size();j++)
		{
			output.push_back(input[i] + " REGEXP '" + keyword[j] + "'");
		}
	}
	return output.size();
}

int paser_and(vector<string> input, vector<string> keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		for(size_t j=0;j<keyword.size();j++)
		{
			output.push_back(input[i] + " and " + keyword[j]);
		}
	}
	return output.size();
}

int paser_is(vector<string> input, vector<string> keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		for(size_t j=0;j<keyword.size();j++)
		{
			output.push_back(input[i] + " = '" + keyword[j] + "'");
		}
	}
	return output.size();
}

int gen_query_condition(string conditions,vector<string> & querystr)
{
	string keycommand,keyvalue;
	stack<string> function_stack;
	stack< vector<string> > value_stack;
	int pos_in = 0;
	int flag_left = 0;
	int flag_right = 0;
	string tempstr;
	for(size_t i=0;i<conditions.size();i++)
	{
		if(conditions[i] == '(')
		{
			tempstr = conditions.substr(pos_in,(i - pos_in));
			function_stack.push(tempstr);
			pos_in = i + 1;
			flag_left = 1;
		}
		if(conditions[i] == ',')
		{
			if (flag_left == 1)
			{
				vector<string> value_v;
				value_v.push_back(conditions.substr(pos_in,(i - pos_in)));
				value_stack.push(value_v);
			}
			pos_in = i + 1;
		}
		if(conditions[i] == ')')
		{
			vector<string> value_v;
			value_v.push_back(conditions.substr(pos_in,(i - pos_in)));
			if (i != (flag_right + 1))
			{
				vector<string> value_v;
				value_v.push_back(conditions.substr(pos_in,(i - pos_in)));
				value_stack.push(value_v);
			}
			pos_in = i + 1;
			flag_left = 0;
			flag_right = i;
			string function = function_stack.top();
			function_stack.pop();
			vector<string> value_getbuildid;
			vector<string> value_right;
			vector<string> value_left;
			if (function == "getbuildid")
			{
				vector_copy(value_stack.top(),value_getbuildid);
				value_stack.pop();
			}else{
				vector_copy(value_stack.top(),value_right);
				value_stack.pop();
				vector_copy(value_stack.top(),value_left);
				value_stack.pop();
			}
			if (function == "getbuildid") 
			{
				vector<string> output;
				paser_getbuildid(value_getbuildid,output);
				value_stack.push(output);
			}
			if (function == "like")
			{
				vector<string> output;
				//cout<<value_left.size()<<endl;
				//cout<<value_right.size()<<endl;
				paser_like(value_left,value_right,output);
				value_stack.push(output);
			}
			if (function == "and")
			{
				vector<string> output;
				paser_and(value_left,value_right,output);
				value_stack.push(output);
			}
			if (function == "is")
			{
				vector<string> output;
				paser_is(value_left,value_right,output);
				value_stack.push(output);
			}
		}
	}
	vector<string> value_temp;
	vector_copy(value_stack.top(),value_temp);
	for(size_t i=0;i<value_temp.size();i++)
	{
		querystr.push_back(value_temp[i]);
	}
	return querystr.size();
}

int query_sql_gen(string Tablename,string DBkeys, string conditions, vector<string> & result)
{
	string s = "select " + DBkeys + " from " + Tablename + " where ";
	vector<string> querystr;
	gen_query_condition(conditions,querystr);
	for(size_t i=0;i<querystr.size();i++)
	{
		result.push_back(s + querystr[i]);
	}
	return result.size();
}

int main()
{
	string conditions = "and(like(group.subject,getbuildid(group.subject)),is(group.report_flag,1))";
	vector<string> result;
	string Tablename = "tzivi";
	string DBkeys = "subject,project";
	query_sql_gen(Tablename,DBkeys,conditions,result);
	for (size_t i=0;i<result.size();i++)
	{
		cout<<result[i]<<endl;
	}
	return 0;
}