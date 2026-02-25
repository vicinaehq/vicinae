import {
	Action,
	ActionPanel,
	Color,
	Detail,
	Form,
	Grid,
	Icon,
	List,
	showToast,
	Toast,
	useNavigation,
} from "@vicinae/api";
import { useState, useEffect } from "react";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

const COLORS = [Color.Red, Color.Blue, Color.Green, Color.Yellow, Color.Orange, Color.Purple, Color.Magenta];
const colorAt = (i: number) => COLORS[((i % COLORS.length) + COLORS.length) % COLORS.length];

function useTick(ms: number) {
	const [tick, setTick] = useState(0);
	useEffect(() => {
		const id = setInterval(() => setTick((t) => t + 1), ms);
		return () => clearInterval(id);
	}, [ms]);
	return tick;
}

// ---------------------------------------------------------------------------
// Test 1 — View-type hot-swap
//
// Cycles the *same* view through List → Grid → Detail → Form.
// Can be triggered manually or put on a 2 s auto-cycle.
// ---------------------------------------------------------------------------

type ViewKind = "list" | "grid" | "detail" | "form";
const VIEW_ORDER: ViewKind[] = ["list", "grid", "detail", "form"];

function ViewTypeSwapper() {
	const [kind, setKind] = useState<ViewKind>("list");
	const [autoCycle, setAutoCycle] = useState(false);
	const tick = useTick(500);

	const cycle = () =>
		setKind((prev) => VIEW_ORDER[(VIEW_ORDER.indexOf(prev) + 1) % VIEW_ORDER.length]);

	useEffect(() => {
		if (!autoCycle) return;
		const id = setInterval(cycle, 2000);
		return () => clearInterval(id);
	}, [autoCycle]);

	const actions = (
		<ActionPanel>
			<Action title="Cycle View Type" icon={Icon.ArrowClockwise} onAction={cycle} />
			<Action
				title={autoCycle ? "Stop Auto-Cycle" : "Start Auto-Cycle (2 s)"}
				icon={Icon.Clock}
				shortcut={{ key: "a", modifiers: ["cmd"] }}
				onAction={() => setAutoCycle((v) => !v)}
			/>
		</ActionPanel>
	);

	switch (kind) {
		case "list":
			return (
				<List navigationTitle={`Hot-Swap: List (t${tick})`} searchBarPlaceholder="List mode...">
					<List.Section title="View Type Swapper">
						<List.Item
							title="Current: List"
							subtitle="Enter to cycle, Cmd+A to auto-cycle"
							icon={Icon.List}
							accessories={[{ tag: { color: Color.Blue, value: "LIST" } }]}
							actions={actions}
						/>
						{Array.from({ length: 4 }, (_, i) => (
							<List.Item
								key={`pad-${i}`}
								title={`Filler ${i} (tick ${tick})`}
								accessories={[{ tag: { color: colorAt(tick + i), value: `${tick}` } }]}
								actions={actions}
							/>
						))}
					</List.Section>
				</List>
			);
		case "grid":
			return (
				<Grid navigationTitle={`Hot-Swap: Grid (t${tick})`} columns={4} searchBarPlaceholder="Grid mode...">
					<Grid.Section title="View Type Swapper">
						{Array.from({ length: 8 }, (_, i) => (
							<Grid.Item
								key={`g-${i}`}
								title={`Cell ${i} (t${tick})`}
								content={{ color: colorAt(tick + i) }}
								actions={actions}
							/>
						))}
					</Grid.Section>
				</Grid>
			);
		case "detail":
			return (
				<Detail
					navigationTitle={`Hot-Swap: Detail (t${tick})`}
					markdown={[
						"# Detail View",
						"",
						`Tick: **${tick}**`,
						"",
						"Press Enter to cycle to Form, or Cmd+A to auto-cycle.",
						"",
						`Current color index: ${tick % COLORS.length}`,
					].join("\n")}
					actions={actions}
				/>
			);
		case "form":
			return (
				<Form navigationTitle={`Hot-Swap: Form (t${tick})`} actions={actions}>
					<Form.Description title="View Type Swapper" text={`Tick ${tick} — press Enter to cycle back to List.`} />
					<Form.TextField id="f1" title="Auto-updating" value={`Value at tick ${tick}`} />
					<Form.Checkbox id="c1" title="Toggle" label={`Tick is even: ${tick % 2 === 0}`} value={tick % 2 === 0} />
					<Form.Dropdown id="d1" title="Color" defaultValue="red">
						{COLORS.map((c) => (
							<Form.Dropdown.Item key={c} value={c} title={c} />
						))}
					</Form.Dropdown>
				</Form>
			);
	}
}

// ---------------------------------------------------------------------------
// Test 2 — Dynamic reactivity
//
// A single list that mutates almost every property on a fast timer:
// titles, subtitles, accessories (color + value), section title,
// item count, loading state, nav title, search placeholder, and
// periodically toggles the detail panel.
// ---------------------------------------------------------------------------

function DynamicReactivity() {
	const tick = useTick(400);
	const [itemCount, setItemCount] = useState(6);
	const showDetail = tick % 20 >= 10;

	const items = Array.from({ length: itemCount }, (_, i) => {
		const phase = ["Alpha", "Beta", "Gamma"][tick % 3];
		return {
			id: `item-${i}`,
			title: `Item ${i} — ${phase} [t${tick}]`,
			subtitle: `Phase ${phase}`,
			accessories: [
				{ tag: { color: colorAt(tick + i), value: `T${tick}` } },
				{ text: { color: colorAt(tick + i + 3), value: `#${(tick * (i + 1)) % 100}` } },
			] as List.Item.Accessory[],
		};
	});

	return (
		<List
			navigationTitle={`Reactive (${itemCount} items, t${tick})`}
			searchBarPlaceholder={`Search phase ${["Alpha", "Beta", "Gamma"][tick % 3]}...`}
			isLoading={tick % 10 < 2}
			isShowingDetail={showDetail}
		>
			<List.Section title={`Dynamic Items (${itemCount}) — tick ${tick}`}>
				{items.map((item) => (
					<List.Item
						key={item.id}
						title={item.title}
						subtitle={item.subtitle}
						accessories={item.accessories}
						detail={
							showDetail ? (
								<List.Item.Detail
									markdown={`**${item.title}**\n\nSubtitle: ${item.subtitle}\n\nTick ${tick}`}
								/>
							) : undefined
						}
						actions={
							<ActionPanel>
								<Action title="Add Item" icon={Icon.Plus} onAction={() => setItemCount((c) => c + 1)} />
								<Action
									title="Remove Item"
									icon={Icon.Minus}
									onAction={() => setItemCount((c) => Math.max(1, c - 1))}
								/>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>

			{tick % 6 < 3 && (
				<List.Section title={`Bonus Section (appears/disappears)`}>
					<List.Item title="I come and go" subtitle={`tick ${tick}`} icon={Icon.Eye} />
				</List.Section>
			)}
		</List>
	);
}

// ---------------------------------------------------------------------------
// Test 3 — Background view mutation
//
// Pushes a deep navigation stack where every view keeps mutating on its own
// timer. The root view also swaps between list ↔ grid periodically.
// The test validates that updates to non-visible views don't leak into the
// currently displayed view.
// ---------------------------------------------------------------------------

function MutatingChild({ label, depth }: { label: string; depth: number }) {
	const tick = useTick(300 + depth * 150);
	const [loading, setLoading] = useState(false);
	const { push } = useNavigation();

	useEffect(() => {
		const id = setInterval(() => setLoading((l) => !l), 1500 + depth * 400);
		return () => clearInterval(id);
	}, [depth]);

	return (
		<List
			navigationTitle={`${label} D${depth} T${tick}`}
			searchBarPlaceholder={`Search ${label} (t${tick})...`}
			isLoading={loading}
		>
			<List.Section title={`${label} — depth ${depth} — tick ${tick}`}>
				<List.Item
					title={`[${label}] depth=${depth} tick=${tick}`}
					subtitle={`loading=${loading}`}
					icon={Icon.Layers}
					actions={
						<ActionPanel>
							<Action
								title={`Push Deeper (depth ${depth + 1})`}
								icon={Icon.ArrowRight}
								onAction={() => push(<MutatingChild label={`Child`} depth={depth + 1} />)}
							/>
							<Action
								title="Toast from here"
								icon={Icon.Bubble}
								onAction={() => showToast(Toast.Style.Success, `From ${label} D${depth} T${tick}`)}
							/>
						</ActionPanel>
					}
				/>
				{Array.from({ length: 5 }, (_, i) => (
					<List.Item
						key={`f-${i}`}
						title={`Filler ${i} (t${tick})`}
						accessories={[{ tag: { color: colorAt(tick + i), value: `D${depth}` } }]}
					/>
				))}
			</List.Section>
		</List>
	);
}

function BackgroundMutation() {
	const tick = useTick(400);
	const [rootKind, setRootKind] = useState<"list" | "grid">("list");
	const { push } = useNavigation();

	// swap root view type every 5 seconds
	useEffect(() => {
		const id = setInterval(() => setRootKind((v) => (v === "list" ? "grid" : "list")), 5000);
		return () => clearInterval(id);
	}, []);

	const pushStack = () => {
		push(<MutatingChild label="Child" depth={1} />);
	};

	const actions = (
		<ActionPanel>
			<Action title="Push Mutating Child" icon={Icon.ArrowRight} onAction={pushStack} />
		</ActionPanel>
	);

	if (rootKind === "grid") {
		return (
			<Grid
				navigationTitle={`Root GRID T${tick}`}
				columns={3}
				searchBarPlaceholder={`Root grid (t${tick})...`}
				isLoading={tick % 7 < 2}
			>
				<Grid.Section title={`Root Grid (t${tick}) — swaps to list every 5 s`}>
					{Array.from({ length: 6 }, (_, i) => (
						<Grid.Item
							key={`rg-${i}`}
							title={`Cell ${i} T${tick}`}
							content={{ color: colorAt(tick + i) }}
							actions={actions}
						/>
					))}
				</Grid.Section>
			</Grid>
		);
	}

	return (
		<List
			navigationTitle={`Root LIST T${tick}`}
			searchBarPlaceholder={`Root list (t${tick})...`}
			isLoading={tick % 7 < 2}
		>
			<List.Section title={`Root List (t${tick}) — swaps to grid every 5 s`}>
				<List.Item
					title={`Root item T${tick}`}
					subtitle="Push to create a mutating child stack"
					icon={Icon.Layers}
					accessories={[{ tag: { color: colorAt(tick), value: "ROOT" } }]}
					actions={actions}
				/>
				{Array.from({ length: 4 }, (_, i) => (
					<List.Item
						key={`rl-${i}`}
						title={`Root filler ${i} (t${tick})`}
						accessories={[{ tag: { color: colorAt(tick + i + 1), value: `${tick}` } }]}
					/>
				))}
			</List.Section>
		</List>
	);
}

// ---------------------------------------------------------------------------
// Test 4 — Combined chaos
//
// Everything at once: the root view mutates AND hot-swaps its view type,
// each pushed child mutates, and every child can also hot-swap itself.
// ---------------------------------------------------------------------------

function ChaosChild({ depth }: { depth: number }) {
	const tick = useTick(250 + depth * 100);
	const [kind, setKind] = useState<ViewKind>("list");
	const [loading, setLoading] = useState(false);
	const { push } = useNavigation();

	// swap view type every 3 s
	useEffect(() => {
		const id = setInterval(
			() => setKind((prev) => VIEW_ORDER[(VIEW_ORDER.indexOf(prev) + 1) % VIEW_ORDER.length]),
			3000 + depth * 500,
		);
		return () => clearInterval(id);
	}, [depth]);

	// toggle loading
	useEffect(() => {
		const id = setInterval(() => setLoading((l) => !l), 2000 + depth * 300);
		return () => clearInterval(id);
	}, [depth]);

	const actions = (
		<ActionPanel>
			<Action
				title={`Push Chaos (depth ${depth + 1})`}
				icon={Icon.ArrowRight}
				onAction={() => push(<ChaosChild depth={depth + 1} />)}
			/>
			<Action
				title="Force Cycle View"
				icon={Icon.ArrowClockwise}
				onAction={() =>
					setKind((prev) => VIEW_ORDER[(VIEW_ORDER.indexOf(prev) + 1) % VIEW_ORDER.length])
				}
			/>
		</ActionPanel>
	);

	const navTitle = `Chaos D${depth} ${kind} T${tick}`;

	switch (kind) {
		case "list":
			return (
				<List navigationTitle={navTitle} searchBarPlaceholder={`Chaos list D${depth} t${tick}`} isLoading={loading}>
					<List.Section title={`Chaos D${depth} List (t${tick})`}>
						{Array.from({ length: 4 }, (_, i) => (
							<List.Item
								key={`cl-${i}`}
								title={`D${depth} Item ${i} [t${tick}]`}
								accessories={[{ tag: { color: colorAt(tick + i), value: kind } }]}
								actions={actions}
							/>
						))}
					</List.Section>
				</List>
			);
		case "grid":
			return (
				<Grid navigationTitle={navTitle} columns={3} searchBarPlaceholder={`Chaos grid D${depth}`} isLoading={loading}>
					<Grid.Section title={`Chaos D${depth} Grid (t${tick})`}>
						{Array.from({ length: 6 }, (_, i) => (
							<Grid.Item
								key={`cg-${i}`}
								title={`D${depth} Cell ${i}`}
								content={{ color: colorAt(tick + i) }}
								actions={actions}
							/>
						))}
					</Grid.Section>
				</Grid>
			);
		case "detail":
			return (
				<Detail
					navigationTitle={navTitle}
					markdown={`# Chaos depth ${depth}\n\nTick **${tick}**, view kind cycles every 3 s.\n\nLoading: ${loading}`}
					actions={actions}
				/>
			);
		case "form":
			return (
				<Form navigationTitle={navTitle} isLoading={loading} actions={actions}>
					<Form.Description title={`Chaos D${depth}`} text={`Tick ${tick}, swapping in ${3 - (tick % 3)} s`} />
					<Form.TextField id="chaos" title="Field" value={`D${depth} T${tick}`} />
				</Form>
			);
	}
}

function CombinedChaos() {
	const { push } = useNavigation();
	const tick = useTick(300);
	const [kind, setKind] = useState<ViewKind>("list");

	useEffect(() => {
		const id = setInterval(
			() => setKind((prev) => VIEW_ORDER[(VIEW_ORDER.indexOf(prev) + 1) % VIEW_ORDER.length]),
			4000,
		);
		return () => clearInterval(id);
	}, []);

	const actions = (
		<ActionPanel>
			<Action
				title="Push Chaos Child"
				icon={Icon.ArrowRight}
				onAction={() => push(<ChaosChild depth={1} />)}
			/>
		</ActionPanel>
	);

	const navTitle = `Chaos Root ${kind} T${tick}`;

	switch (kind) {
		case "list":
			return (
				<List navigationTitle={navTitle} searchBarPlaceholder={`Root list chaos t${tick}`} isLoading={tick % 8 < 2}>
					<List.Section title={`Chaos Root (t${tick})`}>
						<List.Item
							title="Push to start chaos stack"
							subtitle={`Root cycles every 4 s (now: ${kind})`}
							icon={Icon.Bug}
							actions={actions}
						/>
					</List.Section>
				</List>
			);
		case "grid":
			return (
				<Grid navigationTitle={navTitle} columns={4} searchBarPlaceholder={`Root grid chaos`} isLoading={tick % 8 < 2}>
					<Grid.Section title={`Chaos Root Grid (t${tick})`}>
						{Array.from({ length: 4 }, (_, i) => (
							<Grid.Item
								key={`cr-${i}`}
								title={`Root ${i}`}
								content={{ color: colorAt(tick + i) }}
								actions={actions}
							/>
						))}
					</Grid.Section>
				</Grid>
			);
		case "detail":
			return (
				<Detail
					navigationTitle={navTitle}
					markdown={`# Chaos Root\n\nTick **${tick}** — this root swaps view types every 4 s.\n\nPush a child to test background mutation + hot-swap combined.`}
					actions={actions}
				/>
			);
		case "form":
			return (
				<Form navigationTitle={navTitle} isLoading={tick % 8 < 2} actions={actions}>
					<Form.Description title="Chaos Root" text={`Tick ${tick}. Push to start chaos stack.`} />
				</Form>
			);
	}
}

// ---------------------------------------------------------------------------
// Root — pick a torture test
// ---------------------------------------------------------------------------

export default function Torture() {
	return (
		<List searchBarPlaceholder="Pick a torture test..." navigationTitle="Torture Tests">
			<List.Section title="Edge Case Tests">
				<List.Item
					title="View Type Hot-Swap"
					subtitle="Cycle List/Grid/Detail/Form in-place"
					icon={Icon.ArrowClockwise}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<ViewTypeSwapper />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Dynamic Reactivity"
					subtitle="Titles, accessories, sections, detail toggle on timer"
					icon={Icon.Clock}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<DynamicReactivity />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Background View Mutation"
					subtitle="Push stack, background views mutate + root swaps type"
					icon={Icon.Layers}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<BackgroundMutation />} />
						</ActionPanel>
					}
				/>
				<List.Item
					title="Combined Chaos"
					subtitle="Everything at once: all views mutate + hot-swap"
					icon={Icon.Bug}
					actions={
						<ActionPanel>
							<Action.Push title="Run" target={<CombinedChaos />} />
						</ActionPanel>
					}
				/>
			</List.Section>
		</List>
	);
}
