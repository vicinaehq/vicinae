import { List } from "@vicinae/api";
import { useEffect, useState } from "react";

const PAGE_SIZE = 30;
const TOTAL_ITEMS = 200;
const PAGE_DELAY_MS = 600;

type Item = { id: number; title: string };

const fetchPage = (offset: number): Promise<Item[]> =>
	new Promise((resolve) =>
		setTimeout(() => {
			const count = Math.max(0, Math.min(PAGE_SIZE, TOTAL_ITEMS - offset));
			resolve(
				Array.from({ length: count }, (_, i) => ({
					id: offset + i,
					title: `Item ${offset + i + 1}`,
				})),
			);
		}, PAGE_DELAY_MS),
	);

export default function PaginatedList() {
	const [items, setItems] = useState<Item[]>([]);
	const [hasMore, setHasMore] = useState(true);
	const [isLoading, setIsLoading] = useState(true);

	const loadMore = async () => {
		setIsLoading(true);
		const page = await fetchPage(items.length);
		setItems([...items, ...page]);
		setHasMore(items.length + page.length < TOTAL_ITEMS);
		setIsLoading(false);
	};

	useEffect(() => {
		loadMore();
	}, []);

	return (
		<List
			isLoading={isLoading}
			searchBarPlaceholder="Search loaded items..."
			pagination={{ onLoadMore: loadMore, hasMore }}
		>
			<List.Section title={`Loaded ${items.length} of ${TOTAL_ITEMS}`}>
				{items.map((item) => (
					<List.Item
						key={item.id}
						title={item.title}
						subtitle={`#${item.id + 1}`}
					/>
				))}
			</List.Section>
		</List>
	);
}
