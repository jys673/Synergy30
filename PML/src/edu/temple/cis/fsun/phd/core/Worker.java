/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Worker extends Node
{
    private Master master;
    private String id;

    private ArrayList targets = new ArrayList();

    private ArrayList idxSet;
    private ArrayList declS3;
    private ArrayList dDecls;
    private ArrayList vDecls;
    private ArrayList iDecls;

    public Worker(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Worker() { name = "WORKER"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            id = (String) attrs.get("id");
        }

/*
        if (id == null)
            throw new Exception("Invalid <" + name + "> Tag!");
*/
    }

    public boolean isValid()
    {
        if (parent == null)
        {
            Logger.log("Invalid " + name + ": parent node is NOT Parallel!");
            return false;
        }
        if (kids == null || kids.size() == 0)
        {
            Logger.log("Invalid " + name + ": NO kids!");
            return false;
        }
/*
        if (id == null)
        {
            Logger.log("Invalid " + name + ": 'id' is null!");
            return false;
        }
*/
        return true;
    }

    public Code makeCode() throws Exception
    {
        Code closTS1 = new Code();
        closTS1.addCode("\n_close_space(_constructor, \"" + id + "\", 0);");
        this.addChild(closTS1);
        Code closTS2 = new Code();
        closTS2.addCode("\n_close_space(_distributor, \"" + id + "\", 0);");
        this.addChild(closTS2);

        Code tagEnd = new Code();
        tagEnd.addCode("\n/* </worker> */\n");
        this.addChild(tagEnd);

        Code tagStart = new Code();
        tagStart.addCode("\n/* <worker id=\"" + id + "\"> */\n");

        return tagStart;
    }

    public void makeTree() throws Exception
    {
        Logger.log("Worker is completing its tree");

        collectTargets(this);
        orderedTargets();

        setTVarDeclaration();
        setTIdxDeclaration();
        // Commented off. Data declarations will use tag <reference>.
        // setDataDeclaration();
        setIdxSetAndDeclS3();

        Parallel workerPara = (Parallel) parent;
        workerPara.addAtTop(vDecls);    // add Tuple Var decls
        workerPara.addAtTop(iDecls);    // add Tuple Idx decls
        // Commented off. Data declarations will use tag <reference>.
        // workerPara.addAtTop(dDecls);
        workerPara.addAtTop(declS3);

        // Exec body start
        workerPara.addAtTop(createExecStart());

        // Exec body end
        workerPara.addAtEnd(createExecEnd());

        int at = 0;

        Code initTS1 = new Code();
        initTS1.addCode("_distributor = _open_space(\"distributor\", 0, \"" + 
                                                    id + "\");\n");
        initTS1.setParent(this);
        if (at < kids.size()) kids.add(at, initTS1);
        else kids.add(initTS1);
        ++at;

        Code initTS2 = new Code();
        initTS2.addCode("_constructor = _open_space(\"constructor\", 0, \"" + 
                                                    id + "\");\n");
        initTS2.setParent(this);
        if (at < kids.size()) kids.add(at, initTS2);
        else kids.add(initTS2);
        ++at;

        while (at < kids.size() && 
              ((kids.get(at) instanceof Code) && 
              ((Code) kids.get(at)).getCode().toString().trim().length() == 0) || 
              ((kids.get(at) instanceof Data) && 
              ((Data) kids.get(at)).getAction().equals("READ") && 
              ((Data) kids.get(at)).isOnce()))
        {
            ++at;
        }

        // Loop statement start
        if (at < kids.size()) kids.add(at, createLoopStart());
        else kids.add(createLoopStart());
        ++at;

        // Add the token
        Token token = new Token();
        token.setParent(this);
        token.setTargets(targets);
        token.setAction("GET");
        token.setIdxSet(idxSet);
        if (at < kids.size()) kids.add(at, token);
        else kids.add(token);
        ++at;

        // Loop statement end
        kids.add(createLoopEnd());
    }

    private void collectTargets(Node node)
    {
        if (node instanceof Target) targets.add(node);

        ArrayList curKids = node.getKids();
        if (curKids != null)
        {
            for (int i = 0; i < curKids.size(); i++)
            {
                collectTargets((Node) curKids.get(i));
            }
        }
    }

    private void orderedTargets()
    {
        ArrayList oTargets = new ArrayList();

        for (int i = 0; i < targets.size(); i++)
        {
            Target target = (Target) targets.get(i);
            int order = Integer.parseInt(target.getOrder());

            int p = 0;
            while (p < oTargets.size())
            {
                Target oTarget = (Target) oTargets.get(p);
                int oOrder = Integer.parseInt(oTarget.getOrder());

                if (order < oOrder) break;

                p++;
            }
            oTargets.add(p, target);
        }

        targets = oTargets;
    }

    private void setTVarDeclaration()
    {
        vDecls = new ArrayList();

        String vars = "#";
        for (int i = 0; i < kids.size(); i++)
        {
            Node node = (Node) kids.get(i);
            if (node instanceof Data)
            {
                Data data = (Data) node;
                Declaration decl = data.makeTVarDecl();

                String var = decl.getVar() + "+" + decl.getType();
                if (vars.indexOf(var) < 0)
                {
                    decl.setParent(parent);

                    vDecls.add(decl);
                    vars +=var + "#";
                }
            }
        }
    }

    private void setTIdxDeclaration()
    {
        iDecls = new ArrayList();

        String vars = "";
        for (int i = 0; i < kids.size(); i++)
        {
            Node node = (Node) kids.get(i);
            if (node instanceof Data)
            {
                Data data = (Data) node;
                Declaration decl = data.makeTIdxDecl();
                if (decl == null) continue;

                String[] split = decl.getVar().split(":");
                for (int j = 0; j < split.length; j++)
                {
                    String var = split[j];
                    if (vars.indexOf(var) >= 0) continue;

                    decl = new Declaration();
                    decl.setType("int");
                    decl.setVar(var);
                    decl.setParent(parent);

                    iDecls.add(decl);
                    vars +=var + "#";
                }
            }
        }
    }

    private void setDataDeclaration()
    {
        dDecls = new ArrayList();

        String vars = "#";
        for (int i = 0; i < kids.size(); i++)
        {
            Node node = (Node) kids.get(i);
            if (node instanceof Data)
            {
                Data data = (Data) node;
                Declaration decl = data.makeDataDecl();

                String var = decl.getVar() + "+" + decl.getType();
                if (vars.indexOf(var) < 0)
                {
                    decl.setParent(parent);

                    dDecls.add(decl);
                    vars +=var + "#";
                }
            }
        }
    }

    private void setIdxSetAndDeclS3()
    {
        idxSet = new ArrayList();
        declS3 = new ArrayList();

        for (int j = 0; j < targets.size(); j++)
        {
            Target target = (Target) targets.get(j);

            String index = target.getIndex();
            idxSet.add(index);

            Declaration decl = null;
            decl = createIndexVarDecl("_" + index + "_start");
            decl.setParent(parent);
            declS3.add(decl);
            decl = createIndexVarDecl("_" + index + "_stop");
            decl.setParent(parent);
            declS3.add(decl);
            decl = createIndexVarDecl("_" + index + "_step");
            decl.setParent(parent);
            declS3.add(decl);
        }
    }

    private Declaration createIndexVarDecl(String var)
    {
        Declaration start = new Declaration();
        start.setType("int");
        start.setVar(var);
        start.setInit("0");

        return start;
    }

    private Code createExecStart()
    {
        Code execStart = new Code();
        execStart.setParent(parent);
        execStart.setCode(new StringBuffer(
                         "\nmain(int argc, char **argv[])\n{\n"));
        return execStart;
    }

    private Code createExecEnd()
    {
        Code loopEnd = new Code();
        loopEnd.setParent(parent);
        loopEnd.setCode(new StringBuffer("\nexit(0);\n}\n"));
        return loopEnd;
    }

    private Code createLoopStart()
    {
        Code loopStart = new Code();
        loopStart.setParent(this);
        loopStart.setCode(new StringBuffer("\nwhile (1)\n{\n"));
        return loopStart;
    }

    private Code createLoopEnd()
    {
        Code execEnd = new Code();
        execEnd.setParent(this);
        execEnd.setCode(new StringBuffer("\n}\n"));
        return execEnd;
    }

    public ArrayList getTargets() { return this.targets; }

    public ArrayList getIdxSet() { return this.idxSet; }
    public ArrayList getDeclS3() { return this.declS3; }
    public ArrayList getDDecls() { return this.dDecls; }
    public ArrayList getVDecls() { return this.vDecls; }
    public ArrayList getIDecls() { return this.iDecls; }

    public void setMaster(Master master) { this.master = master; }
    public Master getMaster() { return this.master; }

    public void setId(String id) { this.id = id; }
    public String getId() { return this.id; }
}
