using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using Newtonsoft.Json;
using System.Windows.Forms;

namespace WorldEditor
{
    public class MapParser
    {
        public static void writeMap(string filename, Map m)
        {
            JsonSerializer serializer = new JsonSerializer();
            serializer.NullValueHandling = NullValueHandling.Ignore;
            try
            {
                using (StreamWriter sw = new StreamWriter(filename))
                using (JsonWriter writer = new JsonTextWriter(sw))
                {
                    serializer.Serialize(writer, m);
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public static Map readMap(string filename)
        {
            StreamReader sr = new StreamReader(filename);
            string str = sr.ReadToEnd();
            Map m = JsonConvert.DeserializeObject<Map>(str);
            sr.Close();
            return m;
        }

        public static void writeLandscape(string filename, List<Landscape> l)
        {
            JsonSerializer serializer = new JsonSerializer();
            serializer.NullValueHandling = NullValueHandling.Ignore;
            try
            {
                using (StreamWriter sw = new StreamWriter(filename))
                using (JsonWriter writer = new JsonTextWriter(sw))
                {
                    serializer.Serialize(writer, l);
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public static List<Landscape> readLandscape(string filename)
        {
            StreamReader sr = new StreamReader(filename);
            string str = sr.ReadToEnd();
            sr.Close();
            var l = JsonConvert.DeserializeObject<List<Landscape>>(str);
            return l;
        }
    }
}
