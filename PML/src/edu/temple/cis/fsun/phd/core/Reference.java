/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Reference extends Node
{
    private String id;

    // Reference nodes are shared in both Master and Worker trees. This boolean 
    // is used for avoiding adding the tagEnd to kids twice in makeCode method.
    private boolean isCoded = false;

    public Reference(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Reference() { name = "REFERENCE"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            id = (String) attrs.get("id");
        }

        if (id != null && id.equals("*"))
            id = null;
    }

    public boolean isValid()
    {
        if (parent == null || 
          !(parent instanceof Root || parent instanceof Parallel))
        {
            Logger.log("Invalid " + name + ": parent node is NOT Root!");
            return false;
        }
        if (kids == null || !(kids.get(0) instanceof Code))
        {
            Logger.log("Invalid " + name + ": NO referenced code!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        Code tagStart = new Code();
        if (id != null)
        {
            tagStart.addCode("\n/* <reference id=\"" + id + "\"> */\n");
        }
        else
        {
            tagStart.addCode("\n/* <reference> */\n");
        }

        if (!isCoded)
        {
            Code tagEnd = new Code();
            tagEnd.addCode("\n/* </reference> */\n");
            this.addChild(tagEnd);

            isCoded = true;
        }

        return tagStart;
    }

    public void setId(String id) { this.id = id; }
    public String getId() { return this.id; }
}
