using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace WorldEditor
{
    public class CSVParser
    {
        public static int[,] readIntTable(string filepath, ref int rows, ref int cols)
        {
            List<List<string>> ll = new List<List<string>>();
            try
            {
                string strline;
                string[] aryline;
                using(StreamReader mysr = new StreamReader(filepath,
                    System.Text.Encoding.Default))
                {
                    while ((strline = mysr.ReadLine()) != null)
                    {
                        aryline = strline.Split(new char[] { ',' });
                        List<string> li = new List<string>();
                        foreach (string cs in aryline)
                        {
                            li.Add((string)cs.Clone());
                        }
                        ll.Add(li);
                    }
                }
            }
            catch (System.Exception ex)
            {
                Debug.Print("error in reading csv");
                rows = cols = -1;
                return null;
            }
            rows = ll.Count;
            cols = (ll[0]).Count;
            int[,] table = new int[cols, rows];
            try
            {
                for (int i = 0; i < cols; i++)
                {
                    for (int j = 0; j < rows; j++)
                    {
                        table[i, j] = Convert.ToInt32(ll[j][i]);
                    }
                }
            }
            catch (System.Exception ex)
            {
                Debug.Print("error in converting csv to int table");
                return null;
            }
            return table;
        }

        public static void writeIntTable(string filepath, int [,] inttable, int rows, int cols)
        {
            using (StreamWriter sw = new StreamWriter(filepath))
            {
                for (int j = 0; j < rows; j++)
                {
                    for (int i = 0; i < cols-1; i++)
                    {
                        sw.Write(inttable[i, j].ToString() + ",");
                    }
                    sw.Write(inttable[cols - 1, j].ToString());
                    sw.Write("\n");
                }
            }
            
        }
    }
}
