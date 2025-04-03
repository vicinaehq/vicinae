/**
 * @license React
 * react-reconciler-reflection.development.js
 *
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

"use strict";
"production" !== process.env.NODE_ENV &&
  (function () {
    function getNearestMountedFiber(fiber) {
      var node = fiber,
        nearestMounted = fiber;
      if (fiber.alternate) for (; node.return; ) node = node.return;
      else {
        fiber = node;
        do
          (node = fiber),
            0 !== (node.flags & 4098) && (nearestMounted = node.return),
            (fiber = node.return);
        while (fiber);
      }
      return 3 === node.tag ? nearestMounted : null;
    }
    function assertIsMounted(fiber) {
      if (getNearestMountedFiber(fiber) !== fiber)
        throw Error("Unable to find node on an unmounted component.");
    }
    function findCurrentFiberUsingSlowPath(fiber) {
      var alternate = fiber.alternate;
      if (!alternate) {
        alternate = getNearestMountedFiber(fiber);
        if (null === alternate)
          throw Error("Unable to find node on an unmounted component.");
        return alternate !== fiber ? null : fiber;
      }
      for (var a = fiber, b = alternate; ; ) {
        var parentA = a.return;
        if (null === parentA) break;
        var parentB = parentA.alternate;
        if (null === parentB) {
          b = parentA.return;
          if (null !== b) {
            a = b;
            continue;
          }
          break;
        }
        if (parentA.child === parentB.child) {
          for (parentB = parentA.child; parentB; ) {
            if (parentB === a) return assertIsMounted(parentA), fiber;
            if (parentB === b) return assertIsMounted(parentA), alternate;
            parentB = parentB.sibling;
          }
          throw Error("Unable to find node on an unmounted component.");
        }
        if (a.return !== b.return) (a = parentA), (b = parentB);
        else {
          for (var didFindChild = !1, _child = parentA.child; _child; ) {
            if (_child === a) {
              didFindChild = !0;
              a = parentA;
              b = parentB;
              break;
            }
            if (_child === b) {
              didFindChild = !0;
              b = parentA;
              a = parentB;
              break;
            }
            _child = _child.sibling;
          }
          if (!didFindChild) {
            for (_child = parentB.child; _child; ) {
              if (_child === a) {
                didFindChild = !0;
                a = parentB;
                b = parentA;
                break;
              }
              if (_child === b) {
                didFindChild = !0;
                b = parentB;
                a = parentA;
                break;
              }
              _child = _child.sibling;
            }
            if (!didFindChild)
              throw Error(
                "Child was not found in either parent set. This indicates a bug in React related to the return pointer. Please file an issue."
              );
          }
        }
        if (a.alternate !== b)
          throw Error(
            "Return fibers should always be each others' alternates. This error is likely caused by a bug in React. Please file an issue."
          );
      }
      if (3 !== a.tag)
        throw Error("Unable to find node on an unmounted component.");
      return a.stateNode.current === a ? fiber : alternate;
    }
    function findCurrentHostFiberImpl(node) {
      var tag = node.tag;
      if (5 === tag || 26 === tag || 27 === tag || 6 === tag) return node;
      for (node = node.child; null !== node; ) {
        tag = findCurrentHostFiberImpl(node);
        if (null !== tag) return tag;
        node = node.sibling;
      }
      return null;
    }
    function findCurrentHostFiberWithNoPortalsImpl(node) {
      var tag = node.tag;
      if (5 === tag || 26 === tag || 27 === tag || 6 === tag) return node;
      for (node = node.child; null !== node; ) {
        if (
          4 !== node.tag &&
          ((tag = findCurrentHostFiberWithNoPortalsImpl(node)), null !== tag)
        )
          return tag;
        node = node.sibling;
      }
      return null;
    }
    function traverseFragmentInstanceChildren(child, fn, a, b, c) {
      for (; null !== child; ) {
        if (5 === child.tag) {
          if (fn(child.stateNode, a, b, c)) break;
        } else
          (22 === child.tag && null !== child.memoizedState) ||
            traverseFragmentInstanceChildren(child.child, fn, a, b, c);
        child = child.sibling;
      }
    }
    exports.doesFiberContain = function (parentFiber, childFiber) {
      for (
        var parentFiberAlternate = parentFiber.alternate;
        null !== childFiber;

      ) {
        if (childFiber === parentFiber || childFiber === parentFiberAlternate)
          return !0;
        childFiber = childFiber.return;
      }
      return !1;
    };
    exports.findCurrentFiberUsingSlowPath = findCurrentFiberUsingSlowPath;
    exports.findCurrentHostFiber = function (parent) {
      parent = findCurrentFiberUsingSlowPath(parent);
      return null !== parent ? findCurrentHostFiberImpl(parent) : null;
    };
    exports.findCurrentHostFiberWithNoPortals = function (parent) {
      parent = findCurrentFiberUsingSlowPath(parent);
      return null !== parent
        ? findCurrentHostFiberWithNoPortalsImpl(parent)
        : null;
    };
    exports.getContainerFromFiber = function (fiber) {
      return 3 === fiber.tag ? fiber.stateNode.containerInfo : null;
    };
    exports.getNearestMountedFiber = getNearestMountedFiber;
    exports.getSuspenseInstanceFromFiber = function (fiber) {
      if (13 === fiber.tag) {
        var suspenseState = fiber.memoizedState;
        null === suspenseState &&
          ((fiber = fiber.alternate),
          null !== fiber && (suspenseState = fiber.memoizedState));
        if (null !== suspenseState) return suspenseState.dehydrated;
      }
      return null;
    };
    exports.isFiberSuspenseAndTimedOut = function (fiber) {
      var memoizedState = fiber.memoizedState;
      return (
        13 === fiber.tag &&
        null !== memoizedState &&
        null === memoizedState.dehydrated
      );
    };
    exports.traverseFragmentInstance = function (fragmentFiber, fn, a, b, c) {
      traverseFragmentInstanceChildren(fragmentFiber.child, fn, a, b, c);
    };
  })();
