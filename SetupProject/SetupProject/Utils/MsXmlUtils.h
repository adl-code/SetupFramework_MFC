#pragma once
#include <msxml6.h>
#include "Utils\StringUtils.h"
#include <vector>
#include <map>

// Wrapper for MS XML library

namespace Utils
{

	// Forward declaration
	class CMsXmlNode;


	// Class representing an XML document
	class CMsXmlDoc
	{
	private:
		// Set constructor's access to private to avoid direct object creation
		CMsXmlDoc(
			__in IXMLDOMDocument *pXmlDoc);
	public:
		~CMsXmlDoc();
	private:
		// Class data
		IXMLDOMDocument *m_XmlDoc;
	public:
		// Parse from an UTF8-encoded XML string
		static CMsXmlDoc* ParseFromUtf8String(
			__in const char *utf8Xml);
		CMsXmlNode *GetRoot();
	};

	// Representing an XML element
	class CMsXmlNode
	{
	private:
		// Make this constructor private to avoid direct object creation
		// Only CMsXmlDoc class is able to call this constructor
		CMsXmlNode(
			__in IXMLDOMNode *pNode);
		friend CMsXmlDoc;
	public:
		~CMsXmlNode();

	private:
		// Class data
		IXMLDOMNode *m_Node;
	public:
		bool HasChildren();

		// I'm too lazy to implement the iterator interface for children iteration
		long GetChildCount();
		CMsXmlNode *GetChildAtIndex(long idx);
		std::vector<CMsXmlNode*> GetChildren();

		// Get node name
		_tstring GetName();

		// Get node string value
		_tstring GetStringValue();

		// Get attribute map
		std::map<_tstring, _tstring> GetAttributes();

		bool GetAttribute(
			__in LPCTSTR attribName,
			__out _tstring &attribValue);

		// Get node type
		int GetType();
	};


}