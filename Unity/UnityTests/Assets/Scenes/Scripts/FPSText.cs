using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FPSText : MonoBehaviour
{
    public UnityEngine.UI.Text text;

    int counter = 0;
    FrameCounterHelper fps=new FrameCounterHelper();
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        fps.AddFrame();
        text.text = "FPS: "+fps.FPS.ToString()+"/"+ counter.ToString();

        if(Input.GetKeyDown(KeyCode.Space))
        {
            counter++;
        }
    }
}
