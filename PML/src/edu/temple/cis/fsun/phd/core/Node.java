/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public abstract class Node
{
    protected String    name;
    protected Node      parent;
    protected ArrayList kids;

    public void setup(Node p, HashMap attrs) throws Exception
    {
        parent = p;

        if (parent != null) parent.addChild(this);
    }

    public abstract boolean isValid();
    public abstract Code makeCode() throws Exception;

    public void setParent(Node p)    { parent = p; }
    public void setKids(ArrayList k) { kids = k; }

    public void addChild(Node node)
    {
        if (kids == null)
            kids = new ArrayList();

        kids.add(node);

        Logger.log(name + ": add " + node.getName());
    }

    public String    getName()   { return name;   }
    public Node      getParent() { return parent; }
    public ArrayList getKids()   { return kids;   }
}
