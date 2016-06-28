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
        public int[,] Resource { get; set; }
        public int[,] Defense{ get; set; }
        public int[,] Attack{ get; set; }

        public Map(int p_row, int p_col)
        {
            rows = p_row;
            cols = p_col;
            Resource = new int[cols, rows];
            Defense = new int[cols, rows];
            Attack = new int[cols, rows];
        }
    }
}
