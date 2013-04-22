/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;
import java.util.regex.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 * NOTE:
 * 1) the opts, "_MAX" and "_MIN", are set only for scalar data.
 */
public class Data extends Node
{
    private String action;  // (SEND|READ)
    private String var;
    private String type;
    private String opt;     // (ONCE|XCHG|_MAX|_MIN)

    private boolean isArray;
    private boolean isSplit;
    private HashMap array;

    private boolean once;
    private boolean xchg;
    private String dupSpec;

    public Data(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Data() { name = "DATA"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            action = (String) attrs.get("action");
            var    = (String) attrs.get("var");
            type   = (String) attrs.get("type");
            opt    = (String) attrs.get("opt");
        }

        if (action == null || var == null || type == null)
            throw new Exception("Invalid <" + name + "> Tag!");

        parsingAttrs();
    }

    public void parsingAttrs() throws Exception
    {
        once = (opt != null? opt.indexOf("ONCE") >= 0 : false);
        xchg = (opt != null? opt.indexOf("XCHG") >= 0 : false);

        dupSpec = "";
        if (opt != null && opt.indexOf("_") >= 0)
        {
            int p = opt.indexOf("_") + 1;
            dupSpec = "?" + opt.substring(p, p + 3);
        }

        Pattern pattern = Pattern.compile("[\\[\\]]");
        String[] parsed = pattern.split(type);

        if (parsed.length < 1)
            throw new Exception("Invalid 'type' attribute!");

        isArray = (parsed.length > 1);
        if (!isArray) return;

        array = new HashMap();
        array.put("type", parsed[0]);
        for (int i = 1, c = 0; i < parsed.length; i++)
        {
            if (parsed[i] == null || parsed[i].trim().equals(""))
                continue;

            array.put("" + c++, parsed[i].trim());
        }
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
        if (action == null || var == null || type == null)
        {
            Logger.log("Invalid " + name +
                       ": 'action', 'var' or 'type' is null!");
            return false;
        }
        return true;
    }

    public Declaration makeTVarDecl()
    {
        Declaration decl = new Declaration();

        String id = (parent instanceof Master?
                    ((Master) parent).getId() :
                    ((Worker) parent).getId());

        if (isArray)
        {
            String aryType = (String) array.get("type");
            String aryVar = var;

            decl.setType(aryType + " *");
            decl.setVar("_tp_" + aryVar + "_" + id);
        }
        else
        {
            decl.setType(type + " *");
            decl.setVar("_tp_" + var + "_" + id);
        }

        return decl;
    }

    public Declaration makeTIdxDecl()
    {
        if (!isArray) return null;

        Declaration decl = new Declaration();

        String id = "";
        if (parent instanceof Master)
        {
            Pattern pattern = Pattern.compile(".*\\(.*~.*\\).*");
            Matcher matcher = pattern.matcher(type);
            isSplit = matcher.matches();

            id = ((Master) parent).getId();
        }
        else
        {
            Pattern pattern = Pattern.compile(".*\\(.*\\).*");
            Matcher matcher = pattern.matcher(type);
            isSplit = matcher.matches();

            id = ((Worker) parent).getId();
        }

        String allIdx = "";
        for (int i = 0; i < array.size() - 1; i++)
        {
            allIdx += (":" + "_x" + i + "_" + id)+
            (isSplit? (":" + "_y" + i + "_" + id):
                                              "");
        }
        decl.setType("int");
        decl.setVar(allIdx);

        return decl;
    }

    public Declaration makeDataDecl()
    {
        Declaration decl = new Declaration();

        if (isArray)
        {
            String aryType = (String) array.get("type");
            String aryVar = var;
            for (int i = 0; i < array.size() - 1; i++)
            {
                String dimm = (String) array.get("" + i);
                Pattern pattern = Pattern.compile("[\\(\\)]");
                String[] parsed = pattern.split(dimm);

                aryVar += "[" + parsed[0] + "]";
            }

            decl.setType(aryType);
            decl.setVar(aryVar);
        }
        else
        {
            decl.setType(type);
            decl.setVar(var);
        }

        return decl;
    }

    public Code makeCode() throws Exception
    {
        if (parent instanceof Master)
        {
            String id = ((Master) parent).getId();
            if (action.equals("SEND"))
            {
                if (isArray)
                {
                    Pattern pattern = Pattern.compile(".*\\(.*~.*\\).*");
                    Matcher matcher = pattern.matcher(type);

                    if (matcher.matches())
                    {
                        return codeForSendDistributed(true, id);
                    }
                    else
                    {
                        return codeForSendEntireArray(true, id);
                    }
                }
                else
                {
                    return codeForSendScalar(true, id);
                }
            }
            else
            if (action.equals("READ"))
            {
                if (isArray)
                {
                    Pattern pattern = Pattern.compile(".*\\(.*~.*\\).*");
                    Matcher matcher = pattern.matcher(type);

                    if (matcher.matches())
                    {
                        return codeForReadDistributed(true, id);
                    }
                    else
                    {
                        return codeForReadEntireArray(true, id);
                    }
                }
                else
                {
                    return codeForReadScalar(true, id);
                }
            }
        }
        else
        {
            String id = ((Worker) parent).getId();
            if (action.equals("READ"))
            {
                if (isArray)
                {
                    Pattern pattern = Pattern.compile(".*\\(.*\\).*");
                    Matcher matcher = pattern.matcher(type);

                    if (matcher.matches())
                    {
                        return codeForReadDistributed(false, id);
                    }
                    else
                    {
                        return codeForReadEntireArray(false, id);
                    }
                }
                else
                {
                    return codeForReadScalar(false, id);
                }
            }
            else
            if (action.equals("SEND"))
            {
                if (isArray)
                {
                    Pattern pattern = Pattern.compile(".*\\(.*\\).*");
                    Matcher matcher = pattern.matcher(type);

                    if (matcher.matches())
                    {
                        return codeForSendDistributed(false, id);
                    }
                    else
                    {
                        return codeForSendEntireArray(false, id);
                    }
                }
                else
                {
                    return codeForSendScalar(false, id);
                }
            }
        }

        return null;
    }

    private String getTheDataTag()
    {
        return "/* <" + action.toLowerCase() + " var=\"" + var +
               "\" type=\"" + type + (opt != null? "\" opt=\"" + opt : "") +
               "\"/> */";
    }

    private Code codeForSendEntireArray(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_constructor" : "_distributor") :
            (inMaster? "_distributor" : "_constructor");

        Code code = new Code();

        String aryType = (String) array.get("type");

        String line11 = "sprintf(_tp_name, \"" + aryType;
        String line13 = ":" + var + "#%s\"";
        String line15 = ", \"" + id + "\");";

        String line26 = "_tp_size = (";
        String line28 = ") * sizeof(" + aryType + ");";

        String line39 = "_tp_" + var + "_" + id +
                " = (" + aryType + " *)malloc(_tp_size);";

        String line42 = " = ";
        String line44 = ";";

        String line56  = "_status  = _send_data(" + tsName +
                ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line57  = "if (_status < 0) exit(-1);";
        String line58  = "free(" + "_tp_" + var + "_" + id + ");";

        String line12 = "";
        String line14 = "";
        String line27 = "";
        String line40 = "";
        String line43 = "";
        String line45 = "";

        String linear = "";
        String intent = "";
        for (int i = 0; i < array.size() - 1; i++)
        {
            String dimm = (String) array.get("" + i);
            Pattern pattern = Pattern.compile("[\\(\\)]");
            String[] parsed = pattern.split(dimm);

            String size = "(" + parsed[0] + ")";
            String varStart = "0";
            String varStop  = size;
            String varStep  = "1";

            line12 += "(%d)";
            line14 += ", " + size;
            line27 += (i == 0? size : " * " + size);

            String _xi = "_x" + i + "_" + id;

            if (i == 0) linear = _xi;
            else
            if (i == 1) linear = linear + " * " + size + " + " + _xi;
            else  linear = "(" + linear + ")* " + size + " + " + _xi;

            intent += "    ";

            line40 += intent + "for (" + _xi + " = " + varStart +
                                  "; " + _xi + " < " + varStop  +
                                  "; " + _xi + " +=" + varStep  +
                                  ") {\n";
            line43 += (i == 0? var + "[" + _xi + "]" : "[" + _xi + "]");
            line45  = intent + "}\n" + line45;
        }

        String line41 = "_tp_" + var + "_" + id + "[" + linear + "]";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line12 + line13 + line14 + line15 + "\n");
        buffer.append("    " + line26 + line27 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append(line40 + "\n");
        intent += "    ";
        buffer.append(intent + line41 + line42 + line43 + line44 + "\n");
        buffer.append(line45 + "\n");
        buffer.append("    " + line56 + "\n");
        buffer.append("    " + line57 + "\n");
        buffer.append("    " + line58 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    private Code codeForReadEntireArray(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_distributor" : "_constructor") :
            (inMaster? "_constructor" : "_distributor");

        Code code = new Code();

        String aryType = (String) array.get("type");

        String line11 = "sprintf(_tp_name, \"" + aryType;
        String line13 = ":" + var + "#%s\"";
        String line15 = ", \"" + id + "\");";

        String line26 = "_tp_size = (";
        String line28 = ") * sizeof(" + aryType + ");";

        String line39 = "_tp_" + var + "_" + id +
                " = (" + aryType + " *)malloc(_tp_size);";

        String line40  = "_tp_size = _read_data(" + tsName +
                ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line41  = "if (_tp_size < 0) exit(-1);";
        String line58  = "free(" + "_tp_" + var + "_" + id + ");";

        String line53 = " = ";
        String line55 = ";";

        String line12 = "";
        String line14 = "";
        String line27 = "";
        String line51 = "";
        String line52 = "";
        String line56 = "";

        String linear = "";
        String intent = "";
        for (int i = 0; i < array.size() - 1; i++)
        {
            String dimm = (String) array.get("" + i);
            Pattern pattern = Pattern.compile("[\\(\\)]");
            String[] parsed = pattern.split(dimm);

            String size = "(" + parsed[0] + ")";
            String varStart = "0";
            String varStop  = size;
            String varStep  = "1";

            line12 += "(%d)";
            line14 += ", " + size;
            line27 += (i == 0? size : " * " + size);

            String _xi = "_x" + i + "_" + id;

            if (i == 0) linear = _xi;
            else
            if (i == 1) linear = linear + " * " + size + " + " + _xi;
            else  linear = "(" + linear + ")* " + size + " + " + _xi;

            intent += "    ";

            line51 += intent + "for (" + _xi + " = " + varStart +
                                  "; " + _xi + " < " + varStop  +
                                  "; " + _xi + " +=" + varStep  +
                                  ") {\n";
            line52 += (i == 0? var + "[" + _xi + "]" : "[" + _xi + "]");
            line56  = intent + "}\n" + line56;
        }

        String line54 = "_tp_" + var + "_" + id + "[" + linear + "]";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line12 + line13 + line14 + line15 + "\n");
        buffer.append("    " + line26 + line27 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append("    " + line40 + "\n");
        buffer.append("    " + line41 + "\n");
        buffer.append(line51 + "\n");
        intent += "    ";
        buffer.append(intent + line52 + line53 + line54 + line55 + "\n");
        buffer.append(line56 + "\n");
        buffer.append("    " + line58 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    private Code codeForSendDistributed(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_constructor" : "_distributor") :
            (inMaster? "_distributor" : "_constructor");

        Code code = new Code();

        String aryType = (String) array.get("type");

        String line11 = "sprintf(_tp_name, \"" + aryType;
        String line13 = ":" + var + "#%s";
        String line15 = "@%d\"";
        String line17 = ", \"" + id + "\"";
        String line19 = ", sizeof(" + aryType + "));";

        String line26 = "_tp_size = (";
        String line28 = ") * sizeof(" + aryType + ");";

        String line39 = "_tp_" + var + "_" + id +
                " = (" + aryType + " *)malloc(_tp_size);";

        String line42 = " = ";
        String line44 = ";";

        String line56  = "_status  = _send_data(" + tsName +
                ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line57  = "if (_status < 0) exit(-1);";
        String line58  = "free(" + "_tp_" + var + "_" + id + ");";

        String line12 = "";
        String line14 = "";
        String line16 = "";
        String line18 = "";
        String line27 = "";
        String line40 = "";
        String line43 = "";
        String line45 = "";

        String linear = "";
        String intent = "";
        for (int i = 0; i < array.size() - 1; i++)
        {
            String dimm = (String) array.get("" + i);
            String dimmSize = null;
            String size = null;
            String para = null;
            String expr = null;
            String varStart = null;
            String varStop  = null;
            String varStep  = null;

            Pattern ptnStep = Pattern.compile("[\\(\\,\\)]");
            String[] psdStep = ptnStep.split(dimm);
            varStep  = psdStep.length > 2? "(" + psdStep[2] + ")" : "1";
            
            if (dimm.indexOf("~") >= 0)
            {
                Pattern pattern = Pattern.compile("[\\(\\~\\,\\)]");
                String[] parsed = pattern.split(dimm);

                dimmSize = size = "(" + parsed[0] + ")";
                para = "~"; // 'para' != null, for creating correct 'for' loops
                varStart = parsed.length > 1? "(" + parsed[1] + ")" : null;
                varStop  = parsed.length > 2? "(" + parsed[2] + ")" : null;
                //varStep  = "1";

                size = "((" + varStop + " - " + varStart + " - 1) / " +
                                      varStep + " + 1)";
            }
            else
            {
                Pattern pattern = Pattern.compile("[\\(\\:\\,\\)]");
                String[] parsed = pattern.split(dimm);

                dimmSize = size = "(" + parsed[0] + ")";
                para = parsed.length > 1? parsed[1] : null;
                expr = parsed.length > 2? parsed[2] : null;
                varStart = "0";
                varStop  = size;
                //varStep  = "1";

                if (para != null)
                {
                    varStart = "_" + para + "_start";
                    varStop  = "_" + para + "_stop";
                    //varStep  = "_" + para + "_step";

                    if (expr != null)
                    {
                        expr = "(" + expr + ")";
                        if (expr.indexOf(para) >= 0)
                        {
                            varStart = expr.replaceAll(para, varStart);
                            varStop = expr.replaceAll(para, varStop);
                        }
                        else
                        if (expr.indexOf("$L") >= 0)
                        {
                            varStop = varStart;
                            varStart = expr.replaceAll("\\$L", varStop);
                        }
                        else
                        if (expr.indexOf("$H") >= 0)
                        {
                            varStart = varStop;
                            varStop = expr.replaceAll("\\$H", varStart);
                        }
                        else
                        {
                            Logger.log("Warning - " + name +
                                       ": invalid " + expr + "!");
                        }
                    }

                    size = "((" + varStop + " - " + varStart + " - 1) / " +
                                          varStep + " + 1)";
                }
            }

            line12 += "(%d)";
            line14 += "[%d~%d,%d]";
            line16 += ", " + dimmSize;
            line18 += ", " + varStart + ", " + varStop + ", " + varStep;
            line27 += (i == 0? size : " * " + size);

            String _xi = "_x" + i + "_" + id;
            String _yi = "_y" + i + "_" + id;

            if (i == 0) linear = (para != null? _yi : _xi);
            else
            if (i == 1) linear = linear + " * " + size + " + " +
                                (para != null? _yi : _xi);
            else  linear = "(" + linear + ")* " + size + " + " +
                                (para != null? _yi : _xi);

            intent += "    ";

            line40 += intent + "for (" + _xi + " = " + varStart +
                                  (para != null? ", " + _yi + " =0" : "") +
                                  "; " + _xi + " < " + varStop  +
                                  "; " + _xi + " +=" + varStep  +
                                  (para != null? ", " + _yi + " ++" : "") +
                                  ") {\n";

            line43 += (i == 0? var + "[" + _xi + "]" : "[" + _xi + "]");

            line45  = intent + "}\n" + line45;
        }

        String line41 = "_tp_" + var + "_" + id + "[" + linear + "]";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line12 + line13 + line14 + line15 +
                               line16 + line17 + line18 + line19 + "\n");
        buffer.append("    " + line26 + line27 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append(line40 + "\n");
        intent += "    ";
        buffer.append(intent + line41 + line42 + line43 + line44 + "\n");
        buffer.append(line45 + "\n");
        buffer.append("    " + line56 + "\n");
        buffer.append("    " + line57 + "\n");
        buffer.append("    " + line58 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    private Code codeForReadDistributed(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_distributor" : "_constructor") :
            (inMaster? "_constructor" : "_distributor");

        Code code = new Code();

        String aryType = (String) array.get("type");

        String line11 = "sprintf(_tp_name, \"" + aryType;
        String line13 = ":" + var + "#%s";
        String line15 = "@%d\"";
        String line17 = ", \"" + id + "\"";
        String line19 = ", sizeof(" + aryType + "));";

        String line26 = "_tp_size = (";
        String line28 = ") * sizeof(" + aryType + ");";

        String line39 = "_tp_" + var + "_" + id +
                " = (" + aryType + " *)malloc(_tp_size);";

        String line40  = "_tp_size = _read_data(" + tsName +
                ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line41  = "if (_tp_size < 0) exit(-1);";
        String line58  = "free(" + "_tp_" + var + "_" + id + ");";

        String line53 = " = ";
        String line55 = ";";

        String line12 = "";
        String line14 = "";
        String line16 = "";
        String line18 = "";
        String line27 = "";
        String line51 = "";
        String line52 = "";
        String line56 = "";

        String linear = "";
        String intent = "";
        for (int i = 0; i < array.size() - 1; i++)
        {
            String dimm = (String) array.get("" + i);
            String dimmSize = null;
            String size = null;
            String para = null;
            String expr = null;
            String varStart = null;
            String varStop  = null;
            String varStep  = null;

            Pattern ptnStep = Pattern.compile("[\\(\\,\\)]");
            String[] psdStep = ptnStep.split(dimm);
            varStep  = psdStep.length > 2? "(" + psdStep[2] + ")" : "1";
            
            if (dimm.indexOf("~") >= 0)
            {
                Pattern pattern = Pattern.compile("[\\(\\~\\,\\)]");
                String[] parsed = pattern.split(dimm);

                dimmSize = size = "(" + parsed[0] + ")";
                para = "~"; // 'para' != null, for creating correct 'for' loops
                varStart = parsed.length > 1? "(" + parsed[1] + ")" : null;
                varStop  = parsed.length > 2? "(" + parsed[2] + ")" : null;
                //varStep  = "1";

                size = "((" + varStop + " - " + varStart + " - 1) / " +
                                      varStep + " + 1)";
            }
            else
            {
                Pattern pattern = Pattern.compile("[\\(\\:\\,\\)]");
                String[] parsed = pattern.split(dimm);

                dimmSize = size = "(" + parsed[0] + ")";
                para = parsed.length > 1? parsed[1] : null;
                expr = parsed.length > 2? parsed[2] : null;
                varStart = "0";
                varStop  = size;
                //varStep  = "1";

                if (para != null)
                {
                    varStart = "_" + para + "_start";
                    varStop  = "_" + para + "_stop";
                    //varStep  = "_" + para + "_step";

                    if (expr != null)
                    {
                        expr = "(" + expr + ")";
                        if (expr.indexOf(para) >= 0)
                        {
                            varStart = expr.replaceAll(para, varStart);
                            varStop = expr.replaceAll(para, varStop);
                        }
                        else
                        if (expr.indexOf("$L") >= 0)
                        {
                            varStop = varStart;
                            varStart = expr.replaceAll("\\$L", varStop);
                        }
                        else
                        if (expr.indexOf("$H") >= 0)
                        {
                            varStart = varStop;
                            varStop = expr.replaceAll("\\$H", varStart);
                        }
                        else
                        {
                            Logger.log("Warning - " + name +
                                       ": invalid " + expr + "!");
                        }
                    }

                    size = "((" + varStop + " - " + varStart + " - 1) / " +
                                          varStep + " + 1)";
                }
            }

            line12 += "(%d)";
            line14 += "[%d~%d,%d]";
            line16 += ", " + dimmSize;
            line18 += ", " + varStart + ", " + varStop + ", " + varStep;
            line27 += (i == 0? size : " * " + size);

            String _xi = "_x" + i + "_" + id;
            String _yi = "_y" + i + "_" + id;

            if (i == 0) linear = (para != null? _yi : _xi);
            else
            if (i == 1) linear = linear + " * " + size + " + " +
                                (para != null? _yi : _xi);
            else  linear = "(" + linear + ")* " + size + " + " +
                                (para != null? _yi : _xi);

            intent += "    ";

            line51 += intent + "for (" + _xi + " = " + varStart +
                                  (para != null? ", " + _yi + " =0" : "") +
                                  "; " + _xi + " < " + varStop  +
                                  "; " + _xi + " +=" + varStep  +
                                  (para != null? ", " + _yi + " ++" : "") +
                                  ") {\n";

            line52 += (i == 0? var + "[" + _xi + "]" : "[" + _xi + "]");

            line56  = intent + "}\n" + line56;
        }

        String line54 = "_tp_" + var + "_" + id + "[" + linear + "]";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line12 + line13 + line14 + line15 +
                               line16 + line17 + line18 + line19 + "\n");
        buffer.append("    " + line26 + line27 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append("    " + line40 + "\n");
        buffer.append("    " + line41 + "\n");
        buffer.append(line51 + "\n");
        intent += "    ";
        buffer.append(intent + line52 + line53 + line54 + line55 + "\n");
        buffer.append(line56 + "\n");
        buffer.append("    " + line58 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    private Code codeForSendScalar(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_constructor" : "_distributor") :
            (inMaster? "_distributor" : "_constructor");

        Code code = new Code();

        String line11 = "sprintf(_tp_name, \"" + type + ":" + var + "#%s";
        if (dupSpec  != null && dupSpec.length() > 0)
            line11   += (dupSpec + "@%d");
        String line13 = "\", \"" + id + "\"";
        if (dupSpec  != null && dupSpec.length() > 0)
            line13   += ", _tokens";
        String line15 = ");";

        String line26 = "_tp_size = ";
        String line28 = "sizeof(" + type + ");";

        String line39 = "_tp_" + var + "_" + id + " = &" + var +";";

        String line46  = "_status  = _send_data(" + tsName +
                         ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line47  = "if (_status < 0) exit(-1);";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line13 + line15 + "\n");
        buffer.append("    " + line26 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append("    " + line46 + "\n");
        buffer.append("    " + line47 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    private Code codeForReadScalar(boolean inMaster, String id)
    {
        String tsName = xchg?
            (inMaster? "_distributor" : "_constructor") :
            (inMaster? "_constructor" : "_distributor");

        Code code = new Code();

        String line11 = "sprintf(_tp_name, \"" + type + ":" + var + "#%s";
        if (dupSpec  != null && dupSpec.length() > 0)
            line11   += (dupSpec + "@%d");
        String line13 = "\", \"" + id + "\"";
        if (dupSpec  != null && dupSpec.length() > 0)
            line13   += ", _tokens";
        String line15 = ");";

        String line26 = "_tp_size = ";
        String line28 = "sizeof(" + type + ");";

        String line39 = "_tp_" + var + "_" + id + " = &" + var +";";

        String line40  = "_tp_size = _read_data(" + tsName +
                         ", _tp_name, (char *)_tp_" + var + "_" + id + ", _tp_size);";

        String line41  = "if (_tp_size < 0) exit(-1);";

        StringBuffer buffer = new StringBuffer();
        buffer.append("\n");
        buffer.append("    " + getTheDataTag() + "\n");
        buffer.append("    " + line11 + line13 + line15 + "\n");
        buffer.append("    " + line26 + line28 + "\n");
        buffer.append("    " + line39 + "\n");
        buffer.append("    " + line40 + "\n");
        buffer.append("    " + line41 + "\n");
        buffer.append("\n");

        code.setCode(buffer);

        return code;
    }

    public boolean isArray() { return this.isArray; }
    public boolean isSplit() { return this.isSplit; }

    public boolean isOnce() { return this.once; }
    public boolean isXchg() { return this.xchg; }

    public void setAction(String action) { this.action = action; }
    public void setVar   (String var   ) { this.var    = var;    }
    public void setType  (String type  ) { this.type   = type;   }

    public String getAction() { return this.action; }
    public String getVar   () { return this.var;    }
    public String getType  () { return this.type;   }
}
