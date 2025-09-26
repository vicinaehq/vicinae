# Testing

Our current stance about testing:

- we don't unit test QT/GUI stuff because it's not worth it (yet)
- we test stuff that involves parsing/algorithms where a single change can break the app in non-obvious ways, as testing is the obvious solution to the problem.
- if it's too much work to isolate a component in order to test it, maybe don't bother
- if a specific functionnality can be extracted out to its own standalone library it might be worth doing so (such as what we did with `xdgpp`) even if it implies having to do a few types conversion from non-QT to QT types.

We use `catch2` (V3) as our testing framework.

For now we have all tests segmented by sub-library or domain under the `tests/` directory, compiled down to a single test executable that will
run all the test suites by default.

As the number of tests is still low, this is perfectly fine. PRs are still welcome though. 
