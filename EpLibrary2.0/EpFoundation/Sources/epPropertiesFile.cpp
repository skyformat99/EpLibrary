/*! 
PropertiesFile for the EpLibrary
Copyright (C) 2012  Woong Gyu La <juhgiyo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "epPropertiesFile.h"
#include "epException.h"
using namespace epl;

PropertiesFile::PropertiesFile(FileEncodingType encodingType, LockPolicy lockPolicyType):BaseFile(encodingType,lockPolicyType)
{
}

PropertiesFile::PropertiesFile(const PropertiesFile& b):BaseFile(b)
{
	m_propertyList=b.m_propertyList;
}

PropertiesFile::~PropertiesFile()
{
}

bool PropertiesFile::SetProperty(const TCHAR *  key, const TCHAR * val)
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			iter->second=val;
			return true;
		}
	}
	return false;
}

bool PropertiesFile::GetProperty(const TCHAR * key,EpTString &retVal) const
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::const_iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			retVal=iter->second;
			return true;
		}
	}
	return false;
}

EpTString &PropertiesFile::GetProperty(const TCHAR * key)
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			return iter->second;
		}
	}
	EP_VERIFY_OUT_OF_RANGE_W_MSG(0,"Given key does not exists in the list.");
}

const EpTString &PropertiesFile::GetProperty(const TCHAR * key) const
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::const_iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			return iter->second;
		}
	}
	EP_VERIFY_OUT_OF_RANGE_W_MSG(0,"Given key does not exists in the list.");
}

bool PropertiesFile::AddProperty(const TCHAR * key, const TCHAR * val)
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			return false;
		}
	}
	Pair<EpTString,EpTString> insertPair;
	insertPair.first=key;
	insertPair.second=val;
	m_propertyList.push_back(insertPair);
	return true;
}

bool PropertiesFile::RemoveProperty(const TCHAR * key)
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			m_propertyList.erase(iter);
			return true;
		}
	}
	return false;
}

void PropertiesFile::Clear()
{
	LockObj lock(m_lock);
	m_propertyList.clear();
}
void PropertiesFile::writeLoop()
{
	vector<Pair<EpTString,EpTString> >::iterator iter;
	EpTString toFileString;
	for(iter=m_propertyList.begin();iter!=m_propertyList.end();iter++)
	{
		toFileString=_T("");
		toFileString.append(iter->first);
		toFileString.append(iter->second);
		toFileString.append(_T("\r\n"));
		WriteToFile(toFileString.c_str());
	}

}

EpTString& PropertiesFile::operator [](const TCHAR * key)
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			return iter->second;
		}
	}
	Pair<EpTString,EpTString> insertPair;
	insertPair.first=key;
	insertPair.second=_T("");
	m_propertyList.push_back(insertPair);
	return m_propertyList.at(m_propertyList.size()-1).second;
}

const EpTString& PropertiesFile::operator [](const TCHAR * key) const
{
	LockObj lock(m_lock);
	EpTString opKey=key;
	opKey.append(_T("="));
	vector<Pair<EpTString, EpTString> >::const_iterator iter;
	for(iter=m_propertyList.begin();iter != m_propertyList.end();iter++)
	{
		if(iter->first.compare(opKey)==0)
		{
			return iter->second;
		}
	}
	EP_VERIFY_OUT_OF_RANGE(0);
}
void PropertiesFile::loadFromFile(EpTString rest)
{
	EpTString line=_T("");
	while(getLine(rest,line,rest))
	{
		EpTString key;
		EpTString val;
		if(getValueKeyFromLine(line,key,val))
		{
			Pair<EpTString,EpTString> inputPair;
			inputPair.first=key;
			inputPair.second=val;
			m_propertyList.push_back(inputPair);
		}
		else
		{
			Pair<EpTString,EpTString> inputPair;
			inputPair.first=line;
			inputPair.second=_T("");
			m_propertyList.push_back(inputPair);
		}
	}
	
}


bool PropertiesFile::getValueKeyFromLine(EpTString buf, EpTString &retKey, EpTString &retVal)
{
	TCHAR splitChar=0;
	int bufTrav=0;
	if(buf.length()<=0)
		return false;

	retKey=_T("");
	retVal=_T("");

	for(int testTrav=0; testTrav<buf.length();testTrav++)
	{
		if(buf.at(testTrav)==_T('#'))
			return false;
	}

	while(splitChar!=_T('=') && bufTrav<buf.length())
	{
		splitChar=buf.at(bufTrav);
		retKey.append(&splitChar,1);
		bufTrav++;
	}
	buf.erase(0,bufTrav);
	retVal=buf;
	return true;
}