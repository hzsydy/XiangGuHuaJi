using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WorldEditor
{
    public class Map
    {
        public int rows { get; set; }
        public int cols { get; set; }
        public int[,] resources { get; set; }
        public Landscape[,] landscape { get; set; }

        public Map(int p_row, int p_col, Landscape p_landscape)
        {
            rows = p_row;
            cols = p_col;
            landscape = new Landscape[cols, rows];
            resources = new int[cols, rows];
            for (int i = 0; i < cols; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    landscape[i, j] = p_landscape.Clone();
                }
            }
        }
    }
}
