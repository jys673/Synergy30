/*
 *
 */
package edu.temple.cis.fsun.phd.startup;

/**
 *
 */
public class Test
{
    public static void main(String[] args)
    {
        int lStart0 = 0, lStop0 = 4, lStep0 = 1, size0 = 1;
        int tStart0 = 0, tStop0 = 0, tStep0 = 1;
        int oStart0 = 0, oStop0 = 0, oStep0 = 1;
        int lStart1 = 0, lStop1 = 4, lStep1 = 1, size1 = 1;
        int tStart1 = 0, tStop1 = 0, tStep1 = 1;
        int x0, y0, x1, y1, xx; 

        String t = "";

        tStart0 = lStart0; 
        tStep0 = lStep0; 
        for (x0 = lStart0; x0 < lStop0; ) 
         { 
            x0 += (lStep0 * size0) ; 
            tStop0 = (x0 > lStop0? lStop0 : x0) ; 

          oStart0 = tStart0; 
          oStop0 = tStop0; 

            tStart1 = lStart1; 
            tStep1 = lStep1; 
            for (x1 = lStart1, y1 = 1; x1 < lStop1; y1++) 
             { 
                x1 += (lStep1 * size1) ; 
                tStop1 = (x1 > lStop1? lStop1 : x1) ; 
    
                t = "(i:" + tStart0 + "~"+ tStop0 + "," + tStep0 + ")" + 
                    "(j:" + tStart1 + "~"+ tStop1 + "," + tStep1 + ")";
    
                System.out.println("loop A - " + t);

                if (x1 < lStop1) tStart1 = x1; 
    
                tStop0 = tStart0; 
//                tStart0 -= (lStep0 * size0 * y1); 
                tStart0 -= (lStep0 * size0); 
                if (tStart0 < lStart0) break; 
             } 
            if (x0 < lStop0) tStart0 = x0; 
         } 
    
        System.out.println("oStart0 = " + oStart0 + ", oStop0 = " + oStop0);
        tStart0 = oStart0; 
        tStop0 = oStop0; 
        for (xx = lStart1; xx < lStop1; ) 
         { 
            xx += (lStep1 * size1) ; 
            if (xx > lStop1) break ; 
            tStop0 = oStop0; 
    
            tStart1 = xx; 
            tStep1 = lStep1; 
            for (x1 = xx, y1 = 1; x1 < lStop1; y1++) 
             { 
                x1 += (lStep1 * size1) ; 
                tStop1 = (x1 > lStop1? lStop1 : x1) ; 
    
                t = "(i:" + tStart0 + "~"+ tStop0 + "," + tStep0 + ")" + 
                    "(j:" + tStart1 + "~"+ tStop1 + "," + tStep1 + ")";
        
                System.out.println("loop B - " + t);

                if (x1 < lStop1) tStart1 = x1; 
    
                tStop0 = tStart0; 
//                tStart0 -= (lStep0 * size0 * y1); 
               tStart0 -= (lStep0 * size0); 
                if (tStart0 < lStart0) break; 
             } 
            tStart0 = oStart0; 
          }
    }
}
