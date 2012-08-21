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
//#include "service.h"

#include <vector>
#include <string>
#include <map>
//#include <fstream>
#include <iostream>
#include <stack>

using namespace std;


int paser_getbuildid(vector<string> input, vector<string> & output)
{
    for(size_t i=0;i<input.size();i++)
	{
		output.push_back(input[i]);
	}
	return output.size();
}

int paser_like(vector<string> input, string keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		output.push_back(input[i] + " REGEXP '" + keyword + "'");
	}
	return output.size();
}

int paser_and(vector<string> input, vector<string> keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		for(size_t j=0;j<input.size();j++)
		{
			output.push_back(input[i] + " and " + keyword[j]);
		}
	}
	return output.size();
}

int paser_is(vector<string> input, string keyword, vector<string> & output)
{
	for(size_t i=0;i<input.size();i++)
	{
		output.push_back(input[i] + " = '" + keyword + "'");
	}
	return output.size();
}

int gen_query_condition(string conditions,vector<string> & querystr)
{
	string keycommand,keyvalue;
	stack<string> function_stack;
	//stack<string> params_stack;
	stack<string> value_stack;
	int pos_in = 0;
	string tempstr;
	for(size_t i=0;i<conditions.size();i++)
	{
		if(conditions[i] == '(')
		{
			tempstr = conditions.substr(pos_in,(i - pos_in + 1));
			function_stack.push(tempstr);
			pos_in = i + 1;
		}
		if(conditions[i] == ',')
		{
			//vector<string> value_v;
			//value_v.push_back(conditions.substr(pos_in,(i - pos_in + 1)));
			tempstr = conditions.substr(pos_in,(i - pos_in + 1));
			value_stack.push(tempstr);
			pos_in = i + 1;
		}
		if(conditions[i] == ')')
		{
			//vector<string> value_v;
			//value_v.push_back(conditions.substr(pos_in,(i - pos_in + 1)));
			tempstr = conditions.substr(pos_in,(i - pos_in + 1));
			value_stack.push(tempstr);
			pos_in = i + 1;
			string function = function_stack.top();
			function_stack.pop();
			string value_getbuildid;
			string value_right;
			string value_left;
			if (function == "getbuildid")
			{
				value_getbuildid = value_stack.top();
				value_stack.pop();
			}else{
				value_right = value_stack.top();
				value_stack.pop();
				value_left = value_stack.top();
				value_stack.pop();
			}
			if (function == "getbuildid") 
			{
				vector<string> input,output;
				input.push_back(value_getbuildid);
				paser_getbuildid(input,output);
				value_stack.push(output[0]);
			}
			if (function == "like")
			{
				vector<string> input,output;
				input.push_back(value_left);
				string keyword = value_right;
				paser_like(input,keyword,output);
				value_stack.push(output[0]);
			}
			if (function == "and")
			{
				vector<string> input,input_r,output;
				input.push_back(value_left);
				input_r.push_back(value_right);
				paser_and(input,input_r,output);
				value_stack.push(output[0]);
			}
			if (function == "is")
			{
				vector<string> input,output;
				input.push_back(value_left);
				string keyword = value_right;
				paser_is(input,keyword,output);
				value_stack.push(output[0]);
			}
		}
	}
	//vector<string> value_temp = value_stack.top();
	//for(size_t i=0;i<value_temp.size();i++)
	//{
		querystr.push_back(value_stack.top());
	//}
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
	string conditions = "and(getbuildid(group.subject),is(group.report_flag,1))";
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
