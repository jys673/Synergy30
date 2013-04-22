/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Root extends Node
{
    public Root() { name = "ROOT"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(null, null);
    }

    public boolean isValid()
    {
        if (parent != null)
        {
            Logger.log("Invalid " + name + ": parent node is NOT null!");
            return false;
        }
        if (kids == null || kids.size() == 0)
        {
            Logger.log("Invalid " + name + ": NO kids!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        return null;
    }
}
