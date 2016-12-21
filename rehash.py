
from sys import argv
import networkx as nx 

def rehash(filename):
	"""
	Rehashes the input edgelist, nodes start from 1
	"""
	G = nx.read_edgelist(filename, nodetype=int)
	
	mapping = {}
	start = int(argv[2])
	
	for node in G.nodes_iter():
		if node not in mapping:
			mapping[node] = start
			start += 1

	with open('rehashed_{}'.format(filename), 'w') as f:
		for u, v in G.edges_iter():
			f.write('{} {}\n'.format(mapping[u], mapping[v]))

	with open('map_{}'.format(filename), 'w') as f:
		for key, val in mapping.items():
			f.write('{} {}\n'.format(key, val))

	print('Rehashed edgelist at rehashed_{}'.format(filename))
	print('The mapping is at map_{}'.format(filename))


def main():
	if len(argv) < 3:
		print('Enter filename and starting label as command line arg')
		return 

	filename = argv[1]
	rehash(filename)

if __name__ == '__main__':
	main()