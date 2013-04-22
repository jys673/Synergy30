/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

/**
 *
 */
public class Visitor
{
    /**
     * The method provides a place to put actions that are common to
     * all nodes.  Override this in the child visitor class if need to.
     */
    protected void visitSelf(Node node) throws Exception
    {
        // ...
    }
    /**
     * Visit the kids of a node, using the current visitor
     */
    protected void visitKids(Node node) throws Exception
    {
        Iterator iter = node.getKids().iterator();
        while (iter.hasNext())
        {
            Node aKid = (Node) iter.next();
            // ...
        }
    }
}
