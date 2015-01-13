/***************************************************************************
 *
 *  Project
 *
 * Copyright (C) 2013 - 2013, Paul Zhou, <qianlong.zhou@gmail.com>.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>


/*
* \brief Generic configuration Class
*
*/
class Config {
	// Methods
public:

	Config( std::string filename,std::string delimiter = "=",std::string comment = "#" );
	Config();
	template<class T> T Read( const std::string& in_key ) const;  //!<Search for key and read value or optional default value, call as read<T>
	template<class T> T Read( const std::string& in_key, const T& in_value ) const;
	template<class T> bool ReadInto( T& out_var, const std::string& in_key ) const;
	template<class T>
	bool ReadInto( T& out_var, const std::string& in_key, const T& in_value ) const;
	bool FileExist(std::string filename);
	void ReadFile(std::string filename,std::string delimiter = "=",std::string comment = "#" );

	// Check whether key exists in configuration
	bool KeyExists( const std::string& in_key ) const;

	// Modify keys and values
	template<class T> void Add( const std::string& in_key, const T& in_value );
	void Remove( const std::string& in_key );

	// Check or change configuration syntax
	std::string GetDelimiter() const { return delimiter_; }
	std::string GetComment() const { return comment_; }
	std::string SetDelimiter( const std::string& in_s )
	{ std::string old = delimiter_;  delimiter_ = in_s;  return old; }
	std::string SetComment( const std::string& in_s )
	{ std::string old = comment_;  comment_ =  in_s;  return old; }

	// Write or read configuration
	friend std::ostream& operator<<( std::ostream& os, const Config& cf );
	friend std::istream& operator>>( std::istream& is, Config& cf );

protected:
	template<class T> static std::string T_as_string( const T& t );
	template<class T> static T string_as_T( const std::string& s );
	static void Trim( std::string& inout_s );


	// Exception types
public:
	struct File_not_found {
		std::string filename;
		File_not_found( const std::string& filename_ = std::string() )
			: filename(filename_) {} };
		struct Key_not_found {  // thrown only by T read(key) variant of read()
			std::string key;
			Key_not_found( const std::string& key_ = std::string() )
				: key(key_) {} };
	// Data
protected:
    //!< separator between key and value
	std::string delimiter_;
	//!< separator between value and comments
	std::string comment_;
	//!< extracted keys and values
	std::map<std::string,std::string> contents_;

	typedef std::map<std::string,std::string>::iterator mapi;
	typedef std::map<std::string,std::string>::const_iterator mapci;
};


/* static */
template<class T>
std::string Config::T_as_string( const T& t )
{
	// Convert from a T to a string
	// Type T must support << operator
	std::ostringstream ost;
	ost << t;
	return ost.str();
}


/* static */
template<class T>
T Config::string_as_T( const std::string& s )
{
	// Convert from a string to a T
	// Type T must support >> operator
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}


/* static */
template<>
inline std::string Config::string_as_T<std::string>( const std::string& s )
{
	// Convert from a string to a string
	// In other words, do nothing
	return s;
}

/* static */
template<>
inline const char* Config::string_as_T<const char*>( const std::string& s )
{
	// Convert from a string to a string
	// In other words, do nothing
	return s.c_str();
}

/* static */
template<>
inline bool Config::string_as_T<bool>( const std::string& s )
{
	// Convert from a string to a bool
	// Interpret "false", "F", "no", "n", "0" as false
	// Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
	bool b = true;
	std::string sup = s;
	for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
		*p = toupper(*p);  // make string all caps
	if( sup==std::string("FALSE") || sup==std::string("F") ||
		sup==std::string("NO") || sup==std::string("N") ||
		sup==std::string("0") || sup==std::string("NONE") )
		b = false;
	return b;
}


template<class T>
T Config::Read( const std::string& key ) const
{
	// Read the value corresponding to key
	mapci p = contents_.find(key);
	if( p == contents_.end() ) throw Key_not_found(key);
	return string_as_T<T>( p->second );
}


template<class T>
T Config::Read( const std::string& key, const T& value ) const
{
	// Return the value corresponding to key or given default value
	// if key is not found
	mapci p = contents_.find(key);
	if( p == contents_.end() ) return value;
	return string_as_T<T>( p->second );
}


template<class T>
bool Config::ReadInto( T& var, const std::string& key ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise leave var untouched
	mapci p = contents_.find(key);
	bool found = ( p != contents_.end() );
	if( found ) var = string_as_T<T>( p->second );
	return found;
}


template<class T>
bool Config::ReadInto( T& var, const std::string& key, const T& value ) const
{
	// Get the value corresponding to key and store in var
	// Return true if key is found
	// Otherwise set var to given default
	mapci p = contents_.find(key);
	bool found = ( p != contents_.end() );
	if( found )
		var = string_as_T<T>( p->second );
	else
		var = value;
	return found;
}


template<class T>
void Config::Add( const std::string& in_key, const T& value )
{
	// Add a key with given value
	std::string v = T_as_string( value );
	std::string key=in_key;
	Trim(key);
	Trim(v);
	contents_[key] = v;
	return;
}


#endif // CONFIG_H
