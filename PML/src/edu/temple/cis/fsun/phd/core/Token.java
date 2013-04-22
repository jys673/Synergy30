/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Token extends Node
{
    private ArrayList targets;

    private String action;
    private ArrayList idxSet;

    public Token(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Token() { name = "TOKEN"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        // no need for parsing the attributes because the node is not created
        // from a user-defined XML tag but by the transformer.
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
        if (kids != null)
        {
            Logger.log("Invalid " + name + ": kids is NOT null!");
            return false;
        }
        if (action == null || idxSet == null)
        {
            Logger.log("Invalid " + name +
                       ": 'action' or 'idxset' is null!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        Code code = new Code();

        if (action.equals("SET"))
        {
            String id = ((Master) parent).getId();
            String line11 = "sprintf(_tp_name, \"token#%s\", \"" + id + "\");";

            String line44 = "sprintf(_tp_token, \"";
            String line46 = "\"";
            String line48 = ");";

            String equalO = "=";
            int o = -1;

            String line45 = "";
            String line47 = "";
            for (int i = 0; i < idxSet.size(); i++)
            {
                String idx = (String) idxSet.get(i);

                Target target = findTarget(idx);

                String order = target.getOrder();
                String start = target.getStart();
                String stop  = target.getStop();
                String step  = target.getStep();
                String chunk = target.getChunk();
                String jump  = target.getJump();

                if (o == -1) o = Integer.parseInt(order);
                else
                if (o != Integer.parseInt(order))
                    equalO = "!";

                boolean isChunk = (chunk != null? true : false);

                line45 += "(" + idx + ":%d~%d,%d:" +
                      (isChunk? "#" : "^") + "%d)";
                line47 += ", " + start + ", " + stop + ", " + step +
                          ", " + (isChunk? chunk : jump);
            }

            String line26 = "_tp_size = sizeof(_tp_token);";

            String line33 =
"_tokens  = _set_token(_distributor, _tp_name, (char *)_tp_token, _tp_size);";

            String line34  = "if (_tokens < 0) exit(-1);";

            StringBuffer buffer = new StringBuffer();
            buffer.append("\n");
            buffer.append("    " + buildTokenTag() + "\n");
            buffer.append("    " + line11 + "\n");
            buffer.append("    " + line44 + equalO + line45 + line46 + line47 + line48 +
                                            "\n");
            buffer.append("    " + line26 + "\n");
            buffer.append("    " + line33 + "\n");
            buffer.append("    " + line34 + "\n");
            buffer.append("\n");

            code.setCode(buffer);
        }
        else
        if (action.equals("GET"))
        {
            String id = ((Worker) parent).getId();
            String line11 = "sprintf(_tp_name, \"token#%s\", \"" + id + "\");";

            String line26 = "_tp_size = 0;";

            String line33 =
"_tp_size = _get_token(_distributor, _tp_name, (char *)_tp_token, _tp_size);";

            String line34 = "if (_tp_size < 0) exit(-1);";

            String line35 = "if (_tp_token[0] == '!') break;";

            String line44 = "sscanf(_tp_token, \"%d@";
            String line46 = "\", &_tokens";
            String line48 = ");";

            String line45 = "";
            String line47 = "";
            for (int i = 0; i < idxSet.size(); i++)
            {
                String idx = (String) idxSet.get(i);

                line45 += "(" + idx + ":%d~%d,%d)";
                line47 += ", &_" + idx + "_start"+
                          ", &_" + idx + "_stop" +
                          ", &_" + idx + "_step" ;
            }

            StringBuffer buffer = new StringBuffer();
            buffer.append("\n");
            buffer.append("    " + buildTokenTag() + "\n");
            buffer.append("    " + line11 + "\n");
            buffer.append("    " + line26 + "\n");
            buffer.append("    " + line33 + "\n");
            buffer.append("    " + line34 + "\n");
            buffer.append("    " + line35 + "\n");
            buffer.append("    " + line44 + line45 + line46 + line47 + line48 +
                                            "\n");
            buffer.append("\n");

            code.setCode(buffer);
        }

        return code;
    }

    private Target findTarget(String index)
    {
        for (int i = 0; i < targets.size(); i++)
        {
            Target target = (Target) targets.get(i);
            if (target.getIndex().equals(index))
                return target;
        }
        return null;
    }

    private String buildTokenTag()
    {
        String set = "";
        for (int i = 0; i < idxSet.size(); i++)
        {
            String idx = (String) idxSet.get(i);
            set += "(" + idx + ")";
        }

        String tag = "/* <token action=\"" + action +
                            "\" idxset=\"" + set + "\"/> */";
        return tag;
    }

    public void setTargets(ArrayList targets) { this.targets = targets; }
    public ArrayList getTargets() { return this.targets; }

    public void setAction(String action) { this.action = action; }
    public String getAction() { return this.action; }

    public void setIdxSet(ArrayList idxSet) { this.idxSet = idxSet; }
    public ArrayList getIdxSet() { return this.idxSet; }
}
