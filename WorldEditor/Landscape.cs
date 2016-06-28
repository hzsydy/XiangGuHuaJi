using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WorldEditor
{
    public class Landscape
    {
        string Name { get; set; }
        int Attack { get; set; }
        int Defense { get; set; }
        int Effect { get; set; }
        public Landscape(string p_name, int p_attack, int p_defense, int p_effect)
        {
            Name = p_name;
            Attack = p_attack;
            Defense = p_defense;
            Effect = p_effect;
        }
    }
}
