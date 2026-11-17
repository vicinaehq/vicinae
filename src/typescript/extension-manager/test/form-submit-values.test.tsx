import assert from "node:assert/strict";
import { test } from "node:test";
import React, { type ReactElement } from "react";
// must come before @vicinae/api: it reads the runtime globals at import time
import "../src/globals";
import { Action, ActionPanel, Form } from "@vicinae/api";
import { callbackManager } from "../src/callback";
import { createRenderer } from "../src/reconciler";

type Node = Record<string, any>;

const findNode = (
	node: Node | undefined,
	pred: (n: Node) => boolean,
): Node | undefined => {
	if (!node || typeof node !== "object") return undefined;
	if (pred(node)) return node;
	for (const child of node.children ?? []) {
		const found = findNode(child, pred);
		if (found) return found;
	}
	return undefined;
};

/** Renders `element` and returns a way to look at the tree the app would receive. */
const mount = async (element: ReactElement) => {
	let root: Node | undefined;

	const renderer = createRenderer({
		onInitialRender: () => {},
		onUpdate: (views) => {
			// clean (unchanged) views come without a root: keep the last full tree
			root = views.at(-1)?.root ?? root;
		},
	});

	renderer.flushSync(() =>
		renderer.render(React.createElement("slot", null, element)),
	);
	for (let i = 0; i < 50 && !root; i++) {
		await new Promise((r) => setTimeout(r, 20));
	}

	return {
		find: (pred: (n: Node) => boolean) => findNode(root, pred),
		async unmount() {
			renderer.unmount();
			// let the unmount frame flush: the reconciler's frame counter is module-global,
			// so a late frame would make the next test's tree look clean (rootless)
			await new Promise((r) => setTimeout(r, 50));
		},
	};
};

/**
 * Renders a form with a submit action, then does what the app does when it is submitted:
 * it sends the raw field values (plain JSON) to the extension's onSubmit handler.
 */
const submit = async (
	fields: ReactElement,
	payload: Record<string, unknown>,
) => {
	let received: Form.Values | undefined;
	const view = await mount(
		<Form
			actions={
				<ActionPanel>
					<Action.SubmitForm
						onSubmit={(values) => {
							received = values;
						}}
					/>
				</ActionPanel>
			}
		>
			{fields}
		</Form>,
	);

	try {
		const action = view.find((n) => typeof n.onSubmit === "string");
		assert.ok(action, "submit action not found in rendered tree");
		callbackManager.activateHandler(action.onSubmit, [payload]);
		assert.ok(received, "onSubmit was not called");
		return received;
	} finally {
		await view.unmount();
	}
};

const parts = (d: Date) => [
	d.getFullYear(),
	d.getMonth(),
	d.getDate(),
	d.getHours(),
	d.getMinutes(),
];

test("DatePicker value set programmatically (ISO string) is a Date on submit", async () => {
	const iso = "2026-11-13T01:30:37.758Z";
	const values = await submit(
		<Form.DatePicker
			id="date"
			title="Date"
			type={Form.DatePicker.Type.DateTime}
		/>,
		{ date: iso },
	);
	assert.ok(
		values.date instanceof Date,
		`expected a Date, got ${typeof values.date}`,
	);
	assert.equal((values.date as Date).toISOString(), iso);
});

test("DatePicker with time typed in the field ('YYYY-MM-DD HH:mm') is a local Date", async () => {
	const values = await submit(
		<Form.DatePicker id="date" type={Form.DatePicker.Type.DateTime} />,
		{ date: "2026-11-13 10:30" },
	);
	assert.ok(values.date instanceof Date, "expected a Date");
	assert.deepEqual(parts(values.date), [2026, 10, 13, 10, 30]);
});

test("date-only DatePicker ('YYYY-MM-DD') is local midnight, not UTC midnight", async () => {
	const values = await submit(
		<Form.DatePicker id="date" type={Form.DatePicker.Type.Date} />,
		{ date: "2026-11-13" },
	);
	assert.ok(values.date instanceof Date, "expected a Date");
	assert.deepEqual(parts(values.date), [2026, 10, 13, 0, 0]);
	assert.equal(Form.DatePicker.isFullDay(values.date), true);
});

test("empty or invalid DatePicker value is null on submit", async () => {
	for (const bad of ["", null, "not a date"]) {
		const values = await submit(<Form.DatePicker id="date" />, { date: bad });
		assert.equal(values.date, null);
	}
});

test("other fields are not touched by the date conversion", async () => {
	const values = await submit(
		<>
			<Form.TextField id="title" />
			<Form.DatePicker id="date" />
		</>,
		{ title: "2026-11-13", date: "2026-11-13T01:30:37.758Z" },
	);
	assert.equal(values.title, "2026-11-13");
	assert.ok(values.date instanceof Date);
});

test("DatePicker onChange agrees with submit for a date-only value", async () => {
	let changed: Date | null | undefined;
	const view = await mount(
		<Form>
			<Form.DatePicker
				id="date"
				type={Form.DatePicker.Type.Date}
				onChange={(date) => {
					changed = date;
				}}
			/>
		</Form>,
	);
	try {
		const field = view.find((n) => n.$t === "date-picker-field");
		assert.ok(field, "date picker not found in rendered tree");
		callbackManager.activateHandler(field.onChange, ["2026-11-13"]);
		assert.ok(changed instanceof Date, "expected a Date");
		assert.deepEqual(parts(changed), [2026, 10, 13, 0, 0]);
	} finally {
		await view.unmount();
	}
});

test("Action.SubmitForm without onSubmit does not register a submit handler", async () => {
	const view = await mount(
		<Form
			actions={
				<ActionPanel>
					{/* @ts-expect-error Raycast extensions are untyped at runtime */}
					<Action.SubmitForm title="Save" />
				</ActionPanel>
			}
		>
			<Form.DatePicker id="date" />
		</Form>,
	);
	try {
		const action = view.find((n) => n.title === "Save");
		assert.ok(action, "action not found in rendered tree");
		assert.equal(action.onSubmit, undefined);
	} finally {
		await view.unmount();
	}
});
