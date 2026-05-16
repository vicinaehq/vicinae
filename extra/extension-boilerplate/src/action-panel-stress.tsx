import {
	Action,
	ActionPanel,
	Icon,
	List,
	showToast,
	Toast,
	useNavigation,
} from "@vicinae/api";
import { useState, useEffect, useCallback } from "react";

function useTick(ms: number) {
	const [tick, setTick] = useState(0);
	useEffect(() => {
		const id = setInterval(() => setTick((t) => t + 1), ms);
		return () => clearInterval(id);
	}, [ms]);
	return tick;
}

function ReconciliationTest() {
	const tick = useTick(1500);
	const [extra, setExtra] = useState(false);

	const items = Array.from({ length: 8 }, (_, i) => ({
		id: `item-${i}`,
		title: `Item ${i}`,
		subtitle: `Render #${tick}`,
	}));

	return (
		<List
			navigationTitle={`Reconciliation (render #${tick})`}
			searchBarPlaceholder="Select an item and open the action panel..."
		>
			<List.Section title="Items re-render every 1.5s">
				{items.map((item) => (
					<List.Item
						key={item.id}
						title={item.title}
						subtitle={item.subtitle}
						accessories={[{ tag: { value: `r${tick}` } }]}
						actions={
							<ActionPanel title={`Panel for ${item.title}`}>
								<Action
									title={`Primary — render #${tick}`}
									icon={Icon.Star}
									onAction={() =>
										showToast(Toast.Style.Success, `${item.id} at render ${tick}`)
									}
								/>
								<Action
									title={`Secondary — render #${tick}`}
									icon={Icon.Clipboard}
									onAction={() =>
										showToast(Toast.Style.Success, "Secondary action")
									}
								/>
								{extra && (
									<Action
										title={`Extra action (toggled) — render #${tick}`}
										icon={Icon.Plus}
										onAction={() => showToast(Toast.Style.Success, "Extra!")}
									/>
								)}
								<ActionPanel.Section title={`Section — render #${tick}`}>
									<Action
										title={extra ? "Remove extra action" : "Add extra action"}
										icon={extra ? Icon.Minus : Icon.Plus}
										shortcut={{ key: "e", modifiers: ["cmd"] }}
										onAction={() => setExtra((v) => !v)}
									/>
								</ActionPanel.Section>
								<ActionPanel.Submenu
									title="Submenu (should survive re-render)"
									icon={Icon.List}
									shortcut={{ key: "s", modifiers: ["cmd"] }}
								>
									<Action
										title={`Submenu action A — render #${tick}`}
										onAction={() => showToast(Toast.Style.Success, "A")}
									/>
									<Action
										title={`Submenu action B — render #${tick}`}
										onAction={() => showToast(Toast.Style.Success, "B")}
									/>
									<ActionPanel.Section title="Nested">
										<Action
											title={`Nested C — render #${tick}`}
											onAction={() => showToast(Toast.Style.Success, "C")}
										/>
									</ActionPanel.Section>
								</ActionPanel.Submenu>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}

function DeepSubmenuTest() {
	return (
		<List
			navigationTitle="Deep Submenus"
			searchBarPlaceholder="Open the action panel and navigate submenus..."
		>
			<List.Item
				title="Item with deep submenus"
				subtitle="Open panel, navigate 3 levels deep, then Escape back"
				icon={Icon.Layers}
				actions={
					<ActionPanel>
						<Action
							title="Root action"
							icon={Icon.Star}
							onAction={() => showToast(Toast.Style.Success, "Root")}
						/>
						<ActionPanel.Submenu title="Level 1" icon={Icon.ChevronRight}>
							<Action
								title="Level 1 action"
								onAction={() => showToast(Toast.Style.Success, "L1")}
							/>
							<ActionPanel.Submenu title="Level 2" icon={Icon.ChevronRight}>
								<Action
									title="Level 2 action"
									onAction={() => showToast(Toast.Style.Success, "L2")}
								/>
								<ActionPanel.Submenu title="Level 3" icon={Icon.ChevronRight}>
									<Action
										title="Level 3 action A"
										onAction={() => showToast(Toast.Style.Success, "L3-A")}
									/>
									<Action
										title="Level 3 action B"
										onAction={() => showToast(Toast.Style.Success, "L3-B")}
									/>
									<ActionPanel.Section title="Deep section">
										<Action
											title="Level 3 sectioned action"
											onAction={() =>
												showToast(Toast.Style.Success, "L3-section")
											}
										/>
									</ActionPanel.Section>
								</ActionPanel.Submenu>
							</ActionPanel.Submenu>
							<ActionPanel.Section title="L1 extras">
								<Action
									title="L1 extra action"
									onAction={() => showToast(Toast.Style.Success, "L1-extra")}
								/>
							</ActionPanel.Section>
						</ActionPanel.Submenu>
						<ActionPanel.Submenu title="Sibling submenu" icon={Icon.Sidebar}>
							<Action
								title="Sibling action"
								onAction={() => showToast(Toast.Style.Success, "Sibling")}
							/>
						</ActionPanel.Submenu>
					</ActionPanel>
				}
			/>
			<List.Item
				title="Second item (different panel)"
				subtitle="Switching items should replace the panel"
				icon={Icon.Document}
				actions={
					<ActionPanel>
						<Action
							title="Different panel action"
							icon={Icon.Hammer}
							onAction={() => showToast(Toast.Style.Success, "Different panel")}
						/>
					</ActionPanel>
				}
			/>
		</List>
	);
}

function DynamicSubmenuTest() {
	const tick = useTick(2000);
	const [loadedItems, setLoadedItems] = useState<string[]>([]);
	const [loadCount, setLoadCount] = useState(0);

	const handleSubmenuOpen = useCallback(() => {
		setLoadCount((c) => c + 1);
		setLoadedItems([]);
		setTimeout(() => {
			setLoadedItems(["Async item 1", "Async item 2", "Async item 3"]);
		}, 500);
	}, []);

	return (
		<List
			navigationTitle={`Dynamic Submenu (render #${tick})`}
			searchBarPlaceholder="Open a submenu to trigger onOpen..."
		>
			<List.Item
				title="Item with dynamic submenu"
				subtitle={`Loaded ${loadCount} times — list render #${tick}`}
				icon={Icon.Download}
				actions={
					<ActionPanel>
						<Action
							title={`Primary — render #${tick}`}
							icon={Icon.Star}
							onAction={() => showToast(Toast.Style.Success, "Primary")}
						/>
						<ActionPanel.Submenu
							title={`Dynamic submenu (${loadedItems.length} items)`}
							icon={Icon.Download}
							onOpen={handleSubmenuOpen}
						>
							{loadedItems.length === 0 ? (
								<Action title="Loading..." icon={Icon.Clock} onAction={() => {}} />
							) : (
								loadedItems.map((item, i) => (
									<Action
										key={item}
										title={`${item} — render #${tick}`}
										icon={Icon.Checkmark}
										onAction={() => showToast(Toast.Style.Success, item)}
									/>
								))
							)}
							<ActionPanel.Section title="Always present">
								<Action
									title={`Reload count: ${loadCount}`}
									icon={Icon.ArrowClockwise}
									onAction={() => handleSubmenuOpen()}
								/>
							</ActionPanel.Section>
						</ActionPanel.Submenu>
						<ActionPanel.Submenu
							title="Static submenu (control)"
							icon={Icon.List}
						>
							<Action
								title={`Static A — render #${tick}`}
								onAction={() => showToast(Toast.Style.Success, "Static A")}
							/>
							<Action
								title={`Static B — render #${tick}`}
								onAction={() => showToast(Toast.Style.Success, "Static B")}
							/>
						</ActionPanel.Submenu>
					</ActionPanel>
				}
			/>
		</List>
	);
}

function LargeActionPanelTest() {
	const categories = ["Network", "File", "System", "UI", "Data", "Auth"];

	return (
		<List
			navigationTitle="Large Action Panel"
			searchBarPlaceholder="Open the panel and try filtering..."
		>
			<List.Item
				title="Item with many actions"
				subtitle={`${categories.length} sections, ~${categories.length * 15} actions total`}
				icon={Icon.MagnifyingGlass}
				actions={
					<ActionPanel>
						{categories.map((cat) => (
							<ActionPanel.Section key={cat} title={cat}>
								{Array.from({ length: 15 }, (_, i) => (
									<Action
										key={`${cat}-${i}`}
										title={`${cat} Action ${i + 1}`}
										icon={Icon.Dot}
										onAction={() =>
											showToast(
												Toast.Style.Success,
												`${cat} #${i + 1}`,
											)
										}
									/>
								))}
							</ActionPanel.Section>
						))}
						<ActionPanel.Submenu title="Overflow submenu" icon={Icon.Ellipsis}>
							{Array.from({ length: 30 }, (_, i) => (
								<Action
									key={`overflow-${i}`}
									title={`Overflow action ${i + 1}`}
									icon={Icon.Dot}
									onAction={() =>
										showToast(Toast.Style.Success, `Overflow #${i + 1}`)
									}
								/>
							))}
						</ActionPanel.Submenu>
					</ActionPanel>
				}
			/>
		</List>
	);
}

function StressTest() {
	const tick = useTick(500);
	const phase = tick % 4;

	const actionSets: Record<number, { title: string; icon: Icon }[]> = {
		0: [
			{ title: "Alpha", icon: Icon.Star },
			{ title: "Beta", icon: Icon.Hammer },
		],
		1: [
			{ title: "Alpha", icon: Icon.Star },
			{ title: "Beta", icon: Icon.Hammer },
			{ title: "Gamma", icon: Icon.Plus },
		],
		2: [
			{ title: "Gamma", icon: Icon.Plus },
			{ title: "Alpha", icon: Icon.Star },
		],
		3: [
			{ title: "Delta", icon: Icon.LightBulb },
			{ title: "Alpha", icon: Icon.Star },
			{ title: "Beta", icon: Icon.Hammer },
			{ title: "Gamma", icon: Icon.Plus },
		],
	};

	const actions = actionSets[phase];

	return (
		<List
			navigationTitle={`Stress (phase ${phase}, tick ${tick})`}
			searchBarPlaceholder="Open panel — actions change every 500ms"
		>
			{Array.from({ length: 5 }, (_, i) => (
				<List.Item
					key={`s-${i}`}
					title={`Item ${i}`}
					subtitle={`Phase ${phase} — ${actions.length} actions`}
					accessories={[{ tag: { value: `p${phase}` } }]}
					actions={
						<ActionPanel>
							{phase % 2 === 0 ? (
								actions.map((a) => (
									<Action
										key={a.title}
										title={`${a.title} — tick ${tick}`}
										icon={a.icon}
										onAction={() =>
											showToast(Toast.Style.Success, `${a.title} at ${tick}`)
										}
									/>
								))
							) : (
								<>
									<ActionPanel.Section title={`Group A (tick ${tick})`}>
										{actions.slice(0, 2).map((a) => (
											<Action
												key={a.title}
												title={`${a.title} — tick ${tick}`}
												icon={a.icon}
												onAction={() =>
													showToast(Toast.Style.Success, a.title)
												}
											/>
										))}
									</ActionPanel.Section>
									{actions.length > 2 && (
										<ActionPanel.Section title="Group B">
											{actions.slice(2).map((a) => (
												<Action
													key={a.title}
													title={`${a.title} — tick ${tick}`}
													icon={a.icon}
													onAction={() =>
														showToast(Toast.Style.Success, a.title)
													}
												/>
											))}
										</ActionPanel.Section>
									)}
								</>
							)}
							<ActionPanel.Submenu title={`Submenu (phase ${phase})`} icon={Icon.List}>
								{actions.map((a) => (
									<Action
										key={a.title}
										title={`Sub: ${a.title} — tick ${tick}`}
										icon={a.icon}
										onAction={() =>
											showToast(Toast.Style.Success, `Sub: ${a.title}`)
										}
									/>
								))}
							</ActionPanel.Submenu>
						</ActionPanel>
					}
				/>
			))}
		</List>
	);
}

export default function ActionPanelStress() {
	const { push } = useNavigation();

	return (
		<List
			searchBarPlaceholder="Pick an action panel test..."
			navigationTitle="Action Panel Tests"
		>
			<List.Section title="Action Panel Tests">
				<List.Item
					title="Reconciliation"
					subtitle="Panel survives list re-renders, filter & selection preserved"
					icon={Icon.ArrowClockwise}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<ReconciliationTest />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Deep Submenus"
					subtitle="3 levels deep, step-by-step Escape, backspace doesn't pop"
					icon={Icon.Layers}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<DeepSubmenuTest />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Dynamic Submenu"
					subtitle="onOpen loads content, re-renders update open submenu"
					icon={Icon.Download}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<DynamicSubmenuTest />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Large Action Panel"
					subtitle="~90 actions across 6 sections, filter performance"
					icon={Icon.MagnifyingGlass}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<LargeActionPanelTest />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Stress"
					subtitle="500ms re-renders, actions grow/shrink/reorder"
					icon={Icon.Bug}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<StressTest />} />
						</ActionPanel>
					}
				/>
			</List.Section>
		</List>
	);
}
