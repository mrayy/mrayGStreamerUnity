using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class DependencyRoot:MonoBehaviour
{
	bool _isStarted=false;
	List<IDependencyNode> _nodes=new List<IDependencyNode>();
	protected void _OnStarted()
	{
		_isStarted=true;

		foreach (IDependencyNode n in _nodes) {
			n.OnDependencyStart(this);
		}
	}
	public void AddDependencyNode(IDependencyNode node) 
	{
		_nodes.Add (node);
		if (_isStarted)
			node.OnDependencyStart (this);
	}

	protected virtual void Start()
	{
		_OnStarted ();
	}
}

public interface IDependencyNode  {

	void OnDependencyStart(DependencyRoot root);
}
