// search images using FileSearch API
import { Action, ActionPanel, FileSearch, Grid } from "@vicinae/api";
import { basename } from "node:path";
import { useEffect, useRef, useState } from "react";

export default function ImageSearch() {
	const searchRef = useRef<string>("");
	const [files, setFiles] = useState<FileSearch.FileInfo[]>([]);
	const [search, setSearch] = useState("");
	const [isLoading, setIsLoading] = useState(false);

	useEffect(() => {
		searchRef.current = search;
		setIsLoading(true);
		FileSearch.search(search, {
			filters: { category: "Image" },
		})
			.then((ff) => {
				// do not dispatch long request if we moved past it
				if (searchRef.current === search) setFiles(ff);
			})
			.finally(() => {
				if (searchRef.current === search) setIsLoading(false);
			});
	}, [search]);

	return (
		<Grid
			searchBarPlaceholder={"Search for images..."}
			onSearchTextChange={setSearch}
			isLoading={isLoading}
		>
			<Grid.Section
				title={`${files.length} files`}
				fit={Grid.Fit.Contain}
				columns={6}
			>
				{files?.map((file) => (
					<Grid.Item
						title={basename(file.path)}
						content={{ source: file.path }}
						actions={
							<ActionPanel>
								<Action.ShowInFinder path={file.path} />
							</ActionPanel>
						}
					/>
				))}
			</Grid.Section>
		</Grid>
	);
}
