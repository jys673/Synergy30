/*
 *
 */
package edu.temple.cis.fsun.phd;

import java.util.*;

import edu.temple.cis.fsun.phd.core.*;
import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Transformer
{
    private ArrayList references;
    private ArrayList paraTDecls;

    public void transform(Root masterRoot) throws Exception
    {
        validatingTheTree(masterRoot);
        Logger.log("\nTransformation start ... ...\n");

        ArrayList rootKids = masterRoot.getKids();
        for (int i = 0; i < rootKids.size(); i++)
        {
            if (rootKids.get(i) instanceof Parallel)
            {
                Parallel masterPara = (Parallel) rootKids.get(i);
                Master prevMaster = null;

                paraTDecls = new ArrayList();

                ArrayList paraKids = masterPara.getKids();
                for (int j = 0; j < paraKids.size(); j++)
                {
                    if (paraKids.get(j) instanceof Master)
                    {
                        Logger.log("Transformer found a Master");
                        Master master = (Master) paraKids.get(j);

                        // Handling associate master blocks
                        if (prevMaster != null)
                        {
                            prevMaster.setCloseSpace(false);
                            master.setOpenSpace(false);
                        }
                        prevMaster = master;

                        references = new ArrayList();
                        collectReferences(masterRoot, master.getId());

                        ArrayList masterKids = master.getKids();
                        for (int k = 0; k < masterKids.size(); k++)
                        {
                            if (masterKids.get(k) instanceof Worker)
                            {
                                Logger.log("Transformer found a Worker");
                                Worker worker = (Worker) masterKids.remove(k);
                                worker.setId(master.getId());

                                Root workerRoot = new Root();
                                workerRoot.setKids(references);
                                Parallel workerPara = new Parallel(workerRoot);
                                workerPara.setAppName(masterPara.getAppName());
                                workerPara.addChild(worker);
                                worker.setParent(workerPara);
                                worker.setMaster(master);
                                master.setWorker(worker);

                                Logger.log("Transformer built a Worker");
                                worker.makeTree();
                            }
                        }

                        Logger.log("Transformer built a Master");
                        master.makeTree();

                        // collect decls for <parallel>
                        paraTDecls.addAll(master.getVDecls());
                        paraTDecls.addAll(master.getIDecls());
                        paraTDecls.addAll(master.getDeclS3());
                    }
                }
                // filter out duplicate decls
                for (int u = 0; u < paraTDecls.size(); u++)
                {
                    Declaration decl1 = (Declaration) paraTDecls.get(u);
                    boolean dup = false;
                    for (int v = u + 1; v < paraTDecls.size(); v++)
                    {
                        Declaration decl2 = (Declaration) paraTDecls.get(v);
                        if (decl1.isSameAs(decl2))
                        {
                            paraTDecls.remove(v);
                            v--;
                        }
                    }
                }
                masterPara.addAtTop(paraTDecls);
            }
        }

        Logger.log("\nTransformation end ... ...\n");
        validatingTheTree(masterRoot);
    }

    private void validatingTheTree(Node node) throws Exception
    {
        if (!node.isValid())
        {
            throw new Exception("Validation Error: Found a BAD node!");
        }

        if (node instanceof Master && ((Master) node).getWorker() != null)
        {
            Worker worker = ((Master) node).getWorker();
            Root workerRoot = (Root) worker.getParent().getParent();

            validatingTheTree(workerRoot);
        }

        ArrayList curKids = node.getKids();
        if (curKids != null)
        {
            for (int i = 0; i < curKids.size(); i++)
            {
                validatingTheTree((Node) curKids.get(i));
            }
        }
    }

    private void collectReferences(Node node, String id)
    {
        if (node instanceof Reference &&
           ((((Reference) node).getId() == null) ||
            id.equals(((Reference) node).getId())))
        {
            references.add(node);
        }

        ArrayList curKids = node.getKids();
        if (curKids != null)
        {
            for (int i = 0; i < curKids.size(); i++)
            {
                collectReferences((Node) curKids.get(i), id);
            }
        }
    }
}
