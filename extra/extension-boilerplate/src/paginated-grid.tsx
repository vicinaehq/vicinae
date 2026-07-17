import { Grid } from "@vicinae/api";
import { useEffect, useState } from "react";

const PAGE_SIZE = 48;
const TOTAL_ITEMS = 384;
const PAGE_DELAY_MS = 600;

const PALETTE = [
	"#e06c75",
	"#e5c07b",
	"#98c379",
	"#56b6c2",
	"#61afef",
	"#c678dd",
];

type Item = { id: number; title: string; color: string };

const fetchPage = (offset: number): Promise<Item[]> =>
	new Promise((resolve) =>
		setTimeout(() => {
			const count = Math.max(0, Math.min(PAGE_SIZE, TOTAL_ITEMS - offset));
			resolve(
				Array.from({ length: count }, (_, i) => ({
					id: offset + i,
					title: `Item ${offset + i + 1}`,
					color: PALETTE[(offset + i) % PALETTE.length],
				})),
			);
		}, PAGE_DELAY_MS),
	);

export default function PaginatedGrid() {
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
		<Grid
			columns={8}
			isLoading={isLoading}
			searchBarPlaceholder="Search loaded items..."
			pagination={{ onLoadMore: loadMore, hasMore }}
		>
			<Grid.Section title={`Loaded ${items.length} of ${TOTAL_ITEMS}`}>
				{items.map((item) => (
					<Grid.Item
						key={item.id}
						title={item.title}
						content={{ color: item.color }}
					/>
				))}
			</Grid.Section>
		</Grid>
	);
}
