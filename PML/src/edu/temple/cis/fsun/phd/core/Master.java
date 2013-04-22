/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Master extends Node
{
    private Worker worker;
    private String id;

    private boolean closeSpace = true;
    private boolean openSpace = true;

    private ArrayList declS3;
    private ArrayList vDecls;
    private ArrayList iDecls;

    public Master(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Master() { name = "MASTER"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            id = (String) attrs.get("id");
        }

        if (id == null)
            throw new Exception("Invalid <" + name + "> Tag!");
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
        if (id == null)
        {
            Logger.log("Invalid " + name + ": 'id' is null!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        if (closeSpace)
        {
            Code closTS1 = new Code();
            closTS1.addCode("\n_close_space(_constructor, \"" + id + "\", 1);");
            this.addChild(closTS1);
            Code closTS2 = new Code();
            closTS2.addCode("\n_close_space(_distributor, \"" + id + "\", 1);");
            this.addChild(closTS2);
        }

        Code tagEnd = new Code();
        tagEnd.addCode("\n/* </master> */\n");
        this.addChild(tagEnd);

        Code tagStart = new Code();
        tagStart.addCode("\n/* <master id=\"" + id + "\"> */\n");

        return tagStart;
    }

    public void makeTree() throws Exception
    {
        Logger.log("Master is completing its tree");

        setTVarDeclaration();
        setTIdxDeclaration();
        setIdxSetAndDeclS3();

        int at = 0;

        if (openSpace)
        {
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
        }

        while (at < kids.size() && !(kids.get(at) instanceof Data))
        {
            ++at;
        }

        Code clnpSp1 = new Code();
        clnpSp1.addCode("\n_cleanup_space(_distributor, \"" + id + "\");");
        clnpSp1.setParent(this);
        if (at < kids.size()) kids.add(at, clnpSp1);
        else kids.add(clnpSp1);
        ++at;

        Code clnpSp2 = new Code();
        clnpSp2.addCode("\n_cleanup_space(_constructor, \"" + id + "\");");
        clnpSp2.setParent(this);
        if (at < kids.size()) kids.add(at, clnpSp2);
        else kids.add(clnpSp2);
        ++at;

        while (at < kids.size() && 
              (!(kids.get(at) instanceof Data) ||
              ((Data) kids.get(at)).getAction().equals("SEND") &&
              ((Data) kids.get(at)).isOnce()))
        {
            ++at;
        }

        // Add the token
        Token token = new Token();
        token.setParent(this);
        token.setTargets(worker.getTargets());
        token.setAction("SET");
        token.setIdxSet(worker.getIdxSet());
        if (at < kids.size()) kids.add(at, token);
        else kids.add(token);
        ++at;
/*
        Code clnpTS1 = new Code();
        clnpTS1.addCode("_cleanup_space(_distributor, \"" + id + "\");\n");
        clnpTS1.setParent(this);
        this.addChild(clnpTS1);

        Code clnpTS2 = new Code();
        clnpTS2.addCode("_cleanup_space(_constructor, \"" + id + "\");\n");
        clnpTS2.setParent(this);
        this.addChild(clnpTS2);
*/
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

    private void setIdxSetAndDeclS3()
    {
        declS3 = new ArrayList();

        for (int i = 0; i < kids.size(); i++)
        {
            Node node = (Node) kids.get(i);
            if (node instanceof Data)
            {
                Data data = (Data) node;
                if (data.isArray() && data.isSplit())
                {
                    declS3.addAll(worker.getDeclS3());
                    break;
                }
            }
        }
    }

    public ArrayList getDeclS3() { return this.declS3; }
    public ArrayList getVDecls() { return this.vDecls; }
    public ArrayList getIDecls() { return this.iDecls; }

    public void setWorker(Worker worker) { this.worker = worker; }
    public Worker getWorker() { return this.worker; }

    public void setId(String id) { this.id = id; }
    public String getId() { return this.id; }

    public void setCloseSpace(boolean c) { closeSpace = c; }
    public void setOpenSpace(boolean o) { openSpace = o; }
}
