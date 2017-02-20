#include "stdafx.h"
#include "MsXmlUtils.h"

using namespace Utils;

CMsXmlDoc::CMsXmlDoc(
	__in IXMLDOMDocument *pXmlDoc)
	: m_XmlDoc(pXmlDoc)
{
	ASSERT(m_XmlDoc != NULL);
}


CMsXmlDoc::~CMsXmlDoc()
{
	m_XmlDoc->Release();
}

CMsXmlNode *CMsXmlDoc::GetRoot()
{
	IXMLDOMNodeList *children = NULL;
	IXMLDOMNode *root = NULL;
	IXMLDOMElement *docElement = NULL;
	HRESULT hr;

	if (!SUCCEEDED(hr = m_XmlDoc->get_documentElement(&docElement)))
		return NULL;

	if (!SUCCEEDED(hr = docElement->get_childNodes(&children)))
	{
		docElement->Release();
		return NULL;
	}

	long len;
	if (!SUCCEEDED(hr = children->get_length(&len)))
		len = 0;
	for (long i = 0; i < len; i++)
	{
		IXMLDOMNode *childNode = NULL;
		if (SUCCEEDED(hr = children->get_item(i, &childNode)))
		{
			DOMNodeType nodeType;
			if (SUCCEEDED(hr = childNode->get_nodeType(&nodeType)) &&
				nodeType == NODE_ELEMENT)
			{
				root = childNode;
				break;
			}
			childNode->Release();
		}
	}
	
	children->Release();
	docElement->Release();

	if (root == NULL) return NULL;

	return new CMsXmlNode(root);
}

// Parse from an UTF8-encoded XML string
CMsXmlDoc* CMsXmlDoc::ParseFromUtf8String(
	__in const char *utf8Xml)
{
	if (utf8Xml == NULL) return NULL;

	IXMLDOMDocument *pDoc = NULL;
	HRESULT hr;
	if (!SUCCEEDED(hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, __uuidof(pDoc), (LPVOID*)&pDoc)))
		return NULL;

	VARIANT_BOOL loadSucceeded = VARIANT_FALSE;
	BSTR xmlContent = SysAllocStringFromUtf8String(utf8Xml);
	if (xmlContent == NULL)
	{
		pDoc->Release();
		return NULL;
	}

	if (!SUCCEEDED(hr = pDoc->loadXML(xmlContent, &loadSucceeded)) || loadSucceeded != VARIANT_TRUE)
	{
		SysFreeString(xmlContent);
		pDoc->Release();
		return NULL;
	}
	SysFreeString(xmlContent);

	CMsXmlDoc *theDoc = new CMsXmlDoc(pDoc);
	if (theDoc == NULL)
	{
		pDoc->Release();
		return NULL;
	}

	return theDoc;
}

CMsXmlNode::CMsXmlNode(
	__in IXMLDOMNode *pNode)
	: m_Node(pNode)
{
	ASSERT(m_Node != NULL);
}

CMsXmlNode::~CMsXmlNode()
{
	m_Node->Release();
}

bool Utils::CMsXmlNode::HasChildren()
{
	return GetChildCount() == 0;
}

long Utils::CMsXmlNode::GetChildCount()
{
	IXMLDOMNodeList *children;
	HRESULT hr;
	if (!SUCCEEDED(hr = m_Node->get_childNodes(&children)))
		return 0;

	long cnt;
	if (!SUCCEEDED(hr = children->get_length(&cnt)))
		cnt = 0;

	children->Release();
	return cnt;
}

CMsXmlNode *Utils::CMsXmlNode::GetChildAtIndex(long idx)
{
	if (idx < 0) return NULL;

	IXMLDOMNodeList *children;
	HRESULT hr;
	if (!SUCCEEDED(hr = m_Node->get_childNodes(&children)))
		return NULL;

	long cnt;
	if (!SUCCEEDED(hr = children->get_length(&cnt)))
		cnt = 0;

	if (idx >= cnt)
	{
		children->Release();
		return NULL;
	}

	CMsXmlNode *result = NULL;
	IXMLDOMNode *childNode;
	if (SUCCEEDED(hr = children->get_item(idx, &childNode)))
		result = new CMsXmlNode(childNode);

	children->Release();
	return result;
	
}

std::vector<CMsXmlNode*> Utils::CMsXmlNode::GetChildren()
{
	std::vector<CMsXmlNode*> childList;

	IXMLDOMNodeList *children;
	HRESULT hr;
	if (!SUCCEEDED(hr = m_Node->get_childNodes(&children)))
		return childList;

	long cnt;
	if (!SUCCEEDED(hr = children->get_length(&cnt)))
		cnt = 0;

	for (long i = 0; i < cnt; i++)
	{
		
		IXMLDOMNode *childNode;
		if (SUCCEEDED(hr = children->get_item(i, &childNode)))
		{
			CMsXmlNode *node = new CMsXmlNode(childNode);
			if (node)
				childList.push_back(node);
			else
				childNode->Release();
		}
	}
		
	children->Release();
	return childList;
}

// Get node name
_tstring Utils::CMsXmlNode::GetName()
{
	HRESULT hr;
	BSTR name;
	if (!SUCCEEDED(hr = m_Node->get_nodeName(&name)))
		return _T("");

	return UNICODE_TO_TSTRING(name);

}

// Get node string value
_tstring Utils::CMsXmlNode::GetStringValue()
{
	HRESULT hr;
	VARIANT val;
	int nodeType;

	if (SUCCEEDED(hr = m_Node->get_nodeValue(&val)) && val.vt == VT_BSTR)
		return UNICODE_TO_TSTRING(val.bstrVal);
		
	std::vector<CMsXmlNode*> children = GetChildren();
	_tstring result = _T("");
	
	if (children.size() == 1)
	{
		nodeType = children[0]->GetType();
		if (nodeType == NODE_TEXT)
		{
			result = children[0]->GetStringValue();
		}
	}
	for (CMsXmlNode *child : children)
		delete child;
	
	return result;
	
}

// Get attribute map
std::map<_tstring, _tstring> Utils::CMsXmlNode::GetAttributes()
{
	HRESULT hr;
	std::map<_tstring, _tstring> result;

	IXMLDOMNamedNodeMap *nodeMap;
	
	if (!SUCCEEDED(hr = m_Node->get_attributes(&nodeMap)))
		return result;

	long cnt;
	if (!SUCCEEDED(hr = nodeMap->get_length(&cnt)))
		cnt = 0;

	for (long i = 0; i < cnt; i++)
	{
		IXMLDOMNode *node;
		if (SUCCEEDED(hr = nodeMap->get_item(i, &node)))
		{			
			BSTR nodeName;
			VARIANT nodeValue;
			
			if (SUCCEEDED(node->get_nodeName(&nodeName)) &&
				SUCCEEDED(node->get_nodeValue(&nodeValue)) &&
				nodeValue.vt == VT_BSTR)
			{
				result[UNICODE_TO_TSTRING(nodeName)] = UNICODE_TO_TSTRING(nodeValue.bstrVal);
			}
			node->Release();
		}
	}
	nodeMap->Release();

	return result;
}


bool Utils::CMsXmlNode::GetAttribute(
	__in LPCTSTR attribName,
	__out _tstring &attribValue)
{
	if (attribName == NULL) return false;

	BSTR nodeName = SysAllocString(attribName);
	if (nodeName == NULL) return false;

	HRESULT hr;
	IXMLDOMNamedNodeMap *nodeMap;

	if (!SUCCEEDED(hr = m_Node->get_attributes(&nodeMap)))
	{
		SysFreeString(nodeName);
		return false;
	}
	
	IXMLDOMNode *node;
	hr = nodeMap->getNamedItem(nodeName, &node);
	SysFreeString(nodeName);

	bool found = false;
	DOMNodeType nodeType;
	if (SUCCEEDED(hr))
	{		
		VARIANT nodeValue;
		nodeValue.vt = VT_NULL;

		if (SUCCEEDED(hr = node->get_nodeType(&nodeType)) && nodeType == NODE_ATTRIBUTE &&
			SUCCEEDED(hr = node->get_nodeValue(&nodeValue)) && nodeValue.vt == VT_BSTR)
		{
			attribValue = nodeValue.bstrVal;
			found = true;
		}
			
		node->Release();
	}

	nodeMap->Release();

	return found;
}


// Get node type
int Utils::CMsXmlNode::GetType()
{
	HRESULT hr;
	DOMNodeType nodeType;
	if (!SUCCEEDED(hr = m_Node->get_nodeType(&nodeType)))
		nodeType = NODE_INVALID;

	return nodeType;
}
