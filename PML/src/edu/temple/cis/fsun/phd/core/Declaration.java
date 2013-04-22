/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Declaration extends Node
{
    private String type;
    private String var;
    private String init;

    public Declaration(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Declaration() { name = "DECLARATION"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        // no need for parsing the attributes because the node is not created 
        // from a user-defined XML tag but by the transformer.
    }

    public boolean isValid()
    {
        if (parent == null || !(parent instanceof Parallel || 
            parent instanceof Master || parent instanceof Worker))
        {
            Logger.log("Invalid " + name + 
                       ": parent node is NOT Parallel, Master or Worker!");
            return false;
        }
        if (kids != null)
        {
            Logger.log("Invalid " + name + ": kids is NOT null!");
            return false;
        }
        if (type == null || var == null)
        {
            Logger.log("Invalid " + name + ": 'type' or 'var' is null!");
            return false;
        }
        return true;
    }

    public boolean isSameAs(Declaration decl)
    {
        if (type.equals(decl.getType()) && var.equals(decl.getVar()))
            return true;
        return false;
    }

    public Code makeCode() throws Exception
    {
        Code code = new Code();

        if (init != null && init.length() > 0)
            code.addCode(type + " " + var + " = " + init + ";\n");
        else
            code.addCode(type + " " + var + ";\n");

        return code;
    }

    public void setType(String type) { this.type = type; }
    public void setVar (String var ) { this.var  = var;  }
    public void setInit(String init) { this.init = init; }

    public String getType () { return this.type; }
    public String getVar  () { return this.var;  }
    public String getInit () { return this.init; }
}
