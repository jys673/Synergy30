/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;
import java.util.regex.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Target extends Node
{
    private String index;
    private String order;
    private String limits;
    private String chunk;
    private String jump;

    private String start;
    private String stop;
    private String step;

    public Target(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Target() { name = "TARGET"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            index  = (String) attrs.get("index");
            order  = (String) attrs.get("order");
            limits = (String) attrs.get("limits");
            chunk  = (String) attrs.get("chunk");
            jump   = (String) attrs.get("jump");
        }

        if (index == null || order == null || limits == null || 
           (chunk == null && jump  == null))
            throw new Exception("Invalid <" + name + "> Tag!");

        parsingAttrs();
    }

    public void parsingAttrs() throws Exception
    {
        Pattern pattern = Pattern.compile("[\\(\\,\\)]");
        String[] parsed = pattern.split(limits);
        if (parsed.length != 4)
            throw new Exception("Invalid 'limits' attribute!");

        start = parsed[1];
        stop  = parsed[2];
        step  = parsed[3];
    }

    public boolean isValid()
    {
        if (parent == null || 
          !(parent instanceof Master || parent instanceof Worker))
        {
            Logger.log("Invalid " + name + 
                       ": parent node is NOT Master or Worker!");
            return false;
        }
        if (kids == null || !(kids.get(0) instanceof Code))
        {
            Logger.log("Invalid " + name + 
                       ": NO 'for' Stmt wrapped within the tag!");
            return false;
        }
        if (index == null || order == null || chunk == null && jump == null || 
            start == null || stop  == null || step  == null)
        {
            Logger.log("Invalid " + name + 
                    ": 'index', 'order', 'limits' or 'chunk'/'jump' is null!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        Pattern pattern = Pattern.compile("\\s*for\\s*([^;]*;[^;]*;[^;]*)");
        Matcher matcher = pattern.matcher(((Code) kids.get(0)).getCode());

        if (!matcher.matches())
        {
            throw new Exception("NO 'for' Stmt for This <target> Tag!");
        }

        String varStart = "_" + index + "_start";
        String varStop  = "_" + index + "_stop";
        String varStep  = "_" + index + "_step";

        String newFor = "for (" + index + " = " + varStart + "; " + 
                                  index + " < " + varStop  + "; " + 
                                  index + " +=" + varStep  + ") " ;

        StringBuffer buf = new StringBuffer(matcher.replaceFirst(newFor));
        ((Code) kids.get(0)).setCode(buf);

        Code tagStart = new Code();
        tagStart.addCode("\n/* <target index =\"" + index + 
                   (order != null? "\" order =\"" + order : "")+
                                   "\" limits=\"" + limits+ 
                   (chunk != null? "\" chunk =\"" + chunk : 
                                   "\" jump  =\"" + jump )+ 
                                   "\"> */\n");

        Code tagEnd = new Code();
        tagEnd.addCode("\n/* </target> */\n");
        this.addChild(tagEnd);

        return tagStart;
    }

    public void setIndex (String index ) { this.index  = index;  }
    public void setOrder (String order ) { this.order  = order;  }
    public void setLimits(String limits) { this.limits = limits; }
    public void setStart (String start ) { this.start  = start;  }
    public void setStop  (String stop  ) { this.stop   = stop;   }
    public void setStep  (String step  ) { this.step   = step;   }
    public void setChunk (String chunk ) { this.chunk  = chunk;  }
    public void setJump  (String jump  ) { this.jump   = jump;   }

    public String getIndex () { return this.index;  }
    public String getOrder () { return this.order;  }
    public String getLimits() { return this.limits; }
    public String getStart () { return this.start;  }
    public String getStop  () { return this.stop;   }
    public String getStep  () { return this.step;   }
    public String getChunk () { return this.chunk;  }
    public String getJump  () { return this.jump;   }
}
