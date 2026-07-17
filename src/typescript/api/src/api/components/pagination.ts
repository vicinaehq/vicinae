import { useRef } from "react";

// light wrapper around the pagination flow, in which we automatically
// block repeated load more calls from firing if one is already ongoing.
// we don't bother queuing them or anything, maybe one day?
export const usePagination = (options?: {
	onLoadMore: () => PromiseLike<void>;
	hasMore: boolean;
}) => {
	const isLoadingMore = useRef(false);

	return (
		options && {
			onLoadMore: async () => {
				if (!options.hasMore || isLoadingMore.current) return;

				try {
					isLoadingMore.current = true;
					await options.onLoadMore();
				} finally {
					isLoadingMore.current = false;
				}
			},
			hasMore: options.hasMore,
		}
	);
};
