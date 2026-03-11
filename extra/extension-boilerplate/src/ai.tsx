import { AI, List, Action, ActionPanel, showToast, Toast } from "@vicinae/api";
import { useEffect, useRef, useState } from "react";

const SearchSelector = (props: { onModelChange: (s: string) => void }) => {
	const [models, setModels] = useState<AI.Model[]>([]);

	useEffect(() => {
		AI.getModels().then(setModels);
	}, []);

	return (
		<List.Dropdown onChange={props.onModelChange}>
			{models.map((model) => (
				<List.Dropdown.Item
					key={model.id}
					title={model.name}
					value={model.id}
				/>
			))}
		</List.Dropdown>
	);
};

export default function AIPlayground() {
	const [text, setText] = useState("");
	const [detail, setDetail] = useState("");
	const selectedModel = useRef<string | undefined>(undefined);

	const generating = useRef(false);

	const askAI = (prompt: string, options: AI.AskOptions) => {
		return AI.ask(prompt, {
			...options,
			creativity: 2,
			model: selectedModel.current,
		});
	};

	const batchCalls = async (prompt: string, n: number) => {
		const pp = Array.from({ length: n }).map(() => askAI(prompt, {}));
		console.log("batch settled", await Promise.allSettled(pp));
	};

	return (
		<List
			searchText={text}
			isShowingDetail
			searchBarPlaceholder="AI..."
			onSearchTextChange={setText}
			searchBarAccessory={
				<SearchSelector
					onModelChange={(model) => {
						selectedModel.current = model;
					}}
				/>
			}
		>
			<List.Section title={"Models"}></List.Section>
			<List.Item
				title="Try AI.ask"
				detail={<List.Item.Detail markdown={detail} />}
				actions={
					<ActionPanel>
						<Action
							title="AI.ask"
							onAction={async () => {
								if (generating.current) {
									showToast(Toast.Style.Failure, "Already generating...");
									return;
								}

								setText("");
								generating.current = true;
								setDetail("");
								await showToast(Toast.Style.Animated, "Generating...");
								const ask = askAI(text, {});
								ask.on("data", (chunk) => {
									setDetail((detail) => detail + chunk);
								});
								const answer = await ask;
								console.log({ answer });
								await showToast(Toast.Style.Success, "Generated");
								generating.current = false;
							}}
						/>
						<Action title="Batch calls" onAction={() => batchCalls(text, 10)} />
					</ActionPanel>
				}
			/>
		</List>
	);
}
