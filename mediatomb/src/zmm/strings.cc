/*  strings.cc - this file is part of MediaTomb.
                                                                                
    Copyright (C) 2005 Gena Batyan <bgeradz@deadlock.dhs.org>,
                       Sergey Bostandzhyan <jin@deadlock.dhs.org>
                                                                                
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
                                                                                
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                                
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "memory.h"
#include "strings.h"

using namespace zmm;

StringBase::StringBase(int capacity) : Object()
{
	len = capacity;
	data = (char *)MALLOC((len + 1) * sizeof(char));
    store = true;
}
StringBase::StringBase(const char *str) : Object()
{
	len = (int)strlen(str);
	data = (char *)MALLOC((len + 1) * sizeof(char));
	strcpy(data, str);
    store = true;
}
StringBase::StringBase(const char *str, int len) : Object()
{
	this->len = len;
	data = (char *)MALLOC((len + 1) * sizeof(char));
	memcpy(data, str, len);
	data[len] = 0;
    store = true;
}

StringBase::~StringBase()
{
    if (store)
    	FREE(data);
}

bool StringBase::startsWith(StringBase *other)
{
    return (!strncmp(data, other->data, other->len));
}

String::String()
{
	base = NULL;
}
String::String(int capacity)
{
	base = new StringBase(capacity);
	base->retain();
}
String::String(const char *str)
{
	if (str)
	{
		base = new StringBase(str);
		base->retain();
	}
	else
		base = NULL;
}
                
String::String(const char *str, int len)
{
	if(str)
	{
		base = new StringBase(str, len);
		base->retain();
	}
	else
		base = NULL;
}
String::String(const String &other)
{
	base = other.base;
	if(base)
		base->retain();
}
String::String(StringBase *other)
{
	base = other;
	if(base)
		base->retain();
}
String::String(Ref<StringBase> other)
{
    base = other.getPtr();
    if(base)
        base->retain();
}
String::~String()
{
	if(base)
		base->release();
}

int String::length()
{
	if(base)
		return base->len;
	else
		return 0;
}
char *String::c_str()
{
	if(base)
		return base->data;
	else
		return NULL;
}
String String::operator+(String other)
{
	if(! other.base)
		return *this;
	if(! base)
		return other;
	int len = base->len;
	int otherLen = other.base->len;

	String res(len + otherLen);
	strcpy(res.base->data, base->data);
    strcpy(res.base->data + len, other.base->data);
	return res;
}
String String::operator+(const char *other)
{
	if(! other)
		return *this;
	if(! base)
		return String(other);
	int len = base->len;
	int otherLen = (int)strlen(other);
	String res(len + otherLen);
	strcpy(res.base->data, base->data);
	strcpy(res.base->data + len, other);
	return res;
}
String String::operator+(char chr)
{
	char str[2];
	str[0] = chr;
	str[1] = 0;
	return operator+(str);
}

String String::operator+(int x)
{
	return operator+(from(x));
}
String String::operator+(double x)
{
	return operator+(from(x));
}

String String::from(int x)
{
    StringBase *b = new StringBase(MAX_INT_STRING_LENGTH);
	b->len = sprintf(b->data, "%d", x);
    return (String(b));
}
String String::from(long x)
{
    StringBase *b = new StringBase(MAX_LONG_STRING_LENGTH);
	b->len = sprintf(b->data, "%ld", x);
    return (String(b));
}
String String::from(double x)
{
    StringBase *b = new StringBase(MAX_DOUBLE_STRING_LENGTH);
	b->len = sprintf(b->data, "%lf", x);
    return (String(b));
}
String String::allocate(int size)
{
    return String(new StringBase(size));
}
String String::take(const char *data)
{
    StringBase *base = new StringBase();
    base->data = (char *)data;
    base->len = strlen(data);
    base->store = true;
    return String(base);
}
String String::take(const char *data, int length)
{
    StringBase *base = new StringBase();
    base->data = (char *)data;
    base->len = length;
    base->store = true;
    return String(base);
}
String String::refer(const char *data)
{
    StringBase *base = new StringBase();
    base->data = (char *)data;
    base->len = strlen(data);
    base->store = false;
    return String(base);
}
String String::refer(const char *data, int len)
{
    StringBase *base = new StringBase();
    base->data = (char *)data;
    base->len = len;
    base->store = false;
    return String(base);
}
int String::operator==(String other)
{
	if(! base && ! other.base)
		return 1;
	if(base && other.base)
		return ( ! strcmp(base->data, other.base->data) );
	return 0;
}
int String::operator==(const char *other)
{
	if(! base && ! other)
		return 1;
	if(base && other)
		return ( ! strcmp(base->data, other ) );
	return 0;
}

String& String::operator=(String other)
{
	if(base)
		base->release();
	base = other.base;
	if(base)
		base->retain();
	return *this;
}

long String::toLong()
{
	if(! base)
		return 0;
	char *endptr;
	long res = (int)strtol(base->data, &endptr, 10);
	if(*endptr)
        return 0;
    // TODO: throw exception
	return res;
}
double String::toDouble()
{
	if(! base)
		return 0;
	char *endptr;
	double res = strtod(base->data, &endptr);
	if(*endptr)
		return 0;
    // TODO: throw exception
	return res;
}

String String::substring(int from)
{
	if(! base)
		return nil;
	int count = base->len - from;
	return substring(from, count);
}
String String::substring(int from, int count)
{
	if(! base)
		return nil;
	if(from + count > base->len)
		count = base->len - from;
	String res(count);
	strncpy(res.base->data, base->data + from, count);
	*(res.base->data + count) = 0;
	return res;
}
char String::charAt(int index)
{
    return base->data[index];
}
int String::index(char ch)
{
	if(! base)
		return -1;
    char *pos = ::index(base->data, ch);
    if (pos)
        return pos - base->data;
    else
        return -1;
}
int String::rindex(char ch)
{
	if(! base)
		return -1;
    char *pos = ::rindex(base->data, ch);
    if (pos)
        return pos - base->data;
    else
        return -1;
}
