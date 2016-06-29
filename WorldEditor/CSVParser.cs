using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WorldEditor
{
    public class CSVParser
    {
        public static int[,] readIntTable(string filepath, ref int rows, ref int cols)
        {
            try
            {
                string strline;
                string[] aryline;
                List<List<int>> ll = new List<List<int>>();
                StreamReader mysr = new StreamReader(filepath, 
                    System.Text.Encoding.Default);
                int intColCount = 0;
                bool blnFlag = true;
                while ((strline = mysr.ReadLine()) != null)
                {
                    aryline = strline.Split(new char[] { ',' });
                    List<int> li = new List<int>();
                    //妈蛋写不动了
                    //以后再说
                }
            }
            catch (System.Exception ex)
            {
            	
            }
            int[,] table = new int[cols,rows];
            try
            {
                for (int i = 0; i < cols; i++)
                {
                    for (int j = 0; j < rows; j++)
                    {
                        table[i, j] = Convert.ToInt32("0");
                    }
                }
            }
            catch (System.Exception ex)
            {
                return null;
            }
            return table;
        }
    }
}
