/*
 *
 */
package edu.temple.cis.fsun.phd;

import java.util.*;
import java.io.StringReader;

import javax.xml.parsers.SAXParserFactory; 
import javax.xml.parsers.SAXParser;

import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

import edu.temple.cis.fsun.phd.core.*;
import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Parser extends DefaultHandler
{
    public static final String CORE_PATH = "edu.temple.cis.fsun.phd.core.";

    private StringBuffer text;

    private Root theRoot;
    private Node curNode;

    public Root parse(String file) throws Exception
    {
        theRoot = null;
        curNode = null;

        text = null;

        StringBuffer input = new Reader().read(file);
        InputSource iSrc = new InputSource(new StringReader(input.toString()));

        // Use the default (non-validating) parser
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setValidating(true);
        SAXParser saxParser = factory.newSAXParser();
        saxParser.parse(iSrc, this);

        return theRoot;
    }

    //===========================================================
    // SAX DocumentHandler methods
    //===========================================================

    public void startDocument() throws SAXException
    {
        Logger.log("\nParsing start ... ...\n");
    }

    public void endDocument() throws SAXException
    {
        Logger.log("\nParsing end ... ...\n");
    } 

    public void startElement(String namespaceURI, String sName, String qName, 
                             Attributes attrs) throws SAXException
    {
        try
        {
            // parse the code right above the start tag
            parseCodeSection();

            String eName = sName.equals("")? qName : sName;

            HashMap props = new HashMap();
            if (attrs != null)
            {
                for (int i = 0; i < attrs.getLength(); i++)
                {
                    String aName = attrs.getLocalName(i).equals("")? 
                                   attrs.getQName(i) : attrs.getLocalName(i);

                    props.put(aName, attrs.getValue(i));
                }
            }

            curNode = newNode(eName, curNode, props);

            if (curNode instanceof Root) theRoot = (Root) curNode;
        }
        catch (Exception e)
        {
            throw new SAXException(e);
        }
    }

    public void endElement(String namespaceURI, String sName, String qName)
                           throws SAXException
    {
        try
        {
            // parse the code right above the end tag
            parseCodeSection();

            String eName = sName.equals("")? qName : sName;

            String nName = curNode.getName();
            if (curNode instanceof Data) nName = ((Data) curNode).getAction();

            if (!nName.equalsIgnoreCase(eName))
                throw new SAXException("Tag <" + eName + "> NOT Match!");

            curNode = curNode.getParent();
        }
        catch (Exception e)
        {
            throw new SAXException(e);
        }
    }

    public void characters(char buf[], int offset, int len) throws SAXException
    {
        String str = new String(buf, offset, len);

        if (text == null)
        {
            text = new StringBuffer(str);
        }
        else
        {
            text.append(str);
        }
    } 

    //===========================================================
    // Utility Methods ...
    //===========================================================

    private void parseCodeSection() throws Exception
    {
        if (text == null) return;

        Code code = new Code(curNode);
        code.setCode(text);

        text = null;
    } 

    private Node newNode(String tag, Node p, HashMap props) throws Exception
    {
        Node node = null;

        String cName = tag.substring(0, 1).toUpperCase() + 
                       tag.substring(1   ).toLowerCase();

        if (cName.equals("Send"))
        {
            cName = "Data";
            props.put("action", "SEND");
        }
        else
        if (cName.equals("Read"))
        {
            cName = "Data";
            props.put("action", "READ");
        }

        node = (Node) Class.forName(CORE_PATH + cName).newInstance();
        node.setup(p, props);

        Logger.log("Parser creates a " + node.getName());
        return node;
    }

    //===========================================================
    // SAX ErrorHandler methods
    //===========================================================

    public void fatalError(SAXParseException e) throws SAXParseException
    {
        Logger.log("\n* Fatal! " + "Line: " + e.getLineNumber());
        Logger.log(e.getMessage());

        throw e;
    }

    public void error(SAXParseException e) throws SAXParseException
    {
        Logger.log("\n* Error! " + "Line: " + e.getLineNumber());
        Logger.log(e.getMessage());
    }

    public void warning(SAXParseException e) throws SAXParseException
    {
        Logger.log("\nWarning: " + "Line: " + e.getLineNumber());
        Logger.log(e.getMessage());
    }
}
