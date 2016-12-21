import networkx as nx 
import community as cmt
def main():
	G = nx.read_edgelist('./rehashed_karate', nodetype=int)
	best_part = cmt.best_partition(G)
	print(best_part)
	cover = {i: [] for i in range(4)}
	for node, comm_label in best_part.items():
		cover[comm_label].append(node)
	print(cover)

	with open('cover_karate', 'w') as f:
		for _, line in cover.items():
			for node in line:
				f.write('{} '.format(node))
			f.write('-1 \n')

if __name__ == '__main__':
	main()