#pragma once
#include <stack>
#include <utility>
#include <vector>
#include <cmath>

namespace Tiles {
	using tile = std::pair<double, double>;

	double get_volume(const std::vector<tile>& tiles) {
		if (tiles.size() <= 2)
			return 0;

		std::stack<size_t> S;
		double volume = 0;

		for (size_t i = 0; i < tiles.size(); i++) {
			while (S.size() && tiles[S.top()].second < tiles[i].second) {
				size_t prev_top = std::move(S.top());
				S.pop();

				if (S.size()) {
					size_t top_index = S.top();
					volume += std::min(tiles[i].second, tiles[top_index].second - tiles[prev_top].second) * (std::abs(tiles[top_index].first - tiles[i].first));
				}
			}
			S.push(i);
		}

		return volume;
	}
}