using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class DependencyRoot:MonoBehaviour
{
	bool _isStarted=false;
	bool _isDestroyed=false;
	List<IDependencyNode> _nodes=new List<IDependencyNode>();
	protected void _OnStarted()
	{
		_isStarted=true;

		foreach (IDependencyNode n in _nodes) {
			n.OnDependencyStart(this);
		}
	}
	protected void _OnDestroyed()
	{
		_isDestroyed=true;

		foreach (IDependencyNode n in _nodes) {
			n.OnDependencyDestroyed(this);
		}
	}
	public void AddDependencyNode(IDependencyNode node) 
	{
		_nodes.Add (node);
		if (_isStarted)
			node.OnDependencyStart (this);
		if (_isDestroyed)
			node.OnDependencyDestroyed (this);
	}

	protected virtual void Start()
	{
		_OnStarted ();
	}
	protected virtual void OnDestroy()
	{
		_OnDestroyed ();
	}
}

public interface IDependencyNode  {

	void OnDependencyStart(DependencyRoot root);
	void OnDependencyDestroyed(DependencyRoot root);
}
