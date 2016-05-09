# Brillo Common Kernel

## Background

Traditionally, products using the Linux kernel choose a single version
to develop against and, ultimately, for the final release. In order
to support new features, publish security updates, and fix bugs, that
kernel must receive these changes on a regular basis. This becomes an
increasingly difficult burden for product kernel engineers as the chosen
kernel ages. New features, security updates, and bug fixes are being
written for the latest upstream kernel, not a past kernel version. The
older the product kernel becomes, the more time consuming it is to backport
and test changes, and as the number of different product kernels grows,
this becomes an exponential amount of work, from each perspective of
development, review, and test.

In the case of a product having chosen a kernel version matching an
[upstream Long Term Support (LTS) kernel version]
(https://www.kernel.org/category/releases.html), security updates and
bug fixes will be backported by upstream, reducing the burden on product
kernel developers (but does not reduce the burden of testing). The burden
of backporting new features, however, is not reduced. To support the
latest Brillo functionality (which are expected to be regularly updated),
new kernel features will be needed over time, and product kernels will
need to have these kernel features backported. The scope of the work
ends up looking very similar to the non-LTS burden from the previous
paragraph, since product kernels are still aging quickly as the number
of trees proliferates.

Regardless of how feature backports, security updates, and bug fixes are
being accomplished, the product kernel keeps changing, so it needs to
be retested before these changes can be published, no matter what the
development burden has been. As the combinations of devices to chosen
kernel versions to applications grows, the amount of testing required
grows exponentially. Traditionally, Android-based devices fall out of
support before the test burden exceeds engineering resources. However,
since Brillo IoT devices are designed to be supported longer than phones
(and likely with a much longer lifetime tail) this is not a sustainable
way to handle kernel engineering and testing.


## Goal

All Brillo devices will be built from a single common kernel tree
which tracks the latest upstream LTS, _even on released devices_. This
eliminates one of the combinatorial testing variables (the kernel version)
by keeping it the same across all devices. Since each device’s kernel
updates need to be tested anyway, there is no change to the required
level of testing on a per-device basis, but we gain features, security
updates, and bug fixes at a much lower engineering cost.

The Brillo kernel team will keep the Brillo common kernel updated
with the upstream LTS kernel changes, Android-specific patches, and
help forward-port vendor-specific patches. Vendors will no longer need
to spend time on backporting the upstream and Android-specific changes
across multiple product kernel trees and kernel versions. Vendor changes
will be easier to review since they will go through Gerrit instead of
having large sets of patches being pulled into git in bulk.

The Brillo team will be performing a variety of tests and continuous
integration with starter boards on the Brillo kernel tree, so vendors
gain that test coverage automatically as well.

Vendors are expected to develop against upstream, and cherry-pick back
into the Brillo kernel. They’ll need to continually run their own
tests _with real devices_, on the Brillo common kernel and the latest
upstream kernel, preferably [linux-next](https://www.kernel.org/doc/man-pages/linux-next.html).

By developing upstream, vendors will create much more robust code and
will not need to revisit these changes, since they will appear in new
kernels that will be automatically available when the Brillo kernel
moves forward. This also eliminates the need to know when a new LTS
kernel will be available, since changes will already exist in upstream,
and porting to the new LTS will be either automatic or trivial.

In the very rare case where there are vendor-specific changes to be made
that cannot be legitimately upstreamed, they will be easy to review since
they should be mostly confined to driver or SoC areas. In really tight
spots, or for demonstrably required core changes that upstream will not
take, the Brillo kernel team will help wrap features in kernel CONFIGs
to keep vendor-specific changes isolated.

To keep forward-porting easy, the Brillo kernel tree will use common
development processes including regular commit prefix and body fields,
as detailed in the following sections.


# Upstreaming

Vendors should follow the [common conventions for sending Linux kernel
patches](https://www.kernel.org/doc/Documentation/SubmittingPatches),
with special attention given to the "Select the recipients for your patch"
section. If you need, please keep some of the Brillo kernel developers
on CC in case they can help.

For a device to use the Brillo common kernel, having code already present
in the upstream tree is preferred (since it will automatically end up
in the existing or future Brillo common kernel), but when upstream is
ahead of the current LTS, upstream changes can be cherry-picked back to
the Brillo common kernel (and marked as "UPSTREAM" or "BACKPORT" below).

However, frequently after sending code upstream there can be a long lead
time or various other challenges to address before the code will land.
In these cases, a patch series can be added to the Brillo common kernel
tree once it appears on the appropriate upstream development mailing list
(and marked as "FROMLIST" below).


# Brillo kernel commit conventions

## Commit prefix conventions

The Brillo kernel starts its life as an upstream kernel with Android,
Brillo, and vendor commits added on top. To be able to distinguish the
commits easily, we use a commit prefix convention similar to Chrome OS:

* "UPSTREAM: " The commit comes from upstream from a later kernel version,
  and its original SHA is noted in a "cherry-picked from ..." line at
  the end of the body of the commit message, before the committer's
  Signed-off-by line.
* "BACKPORT: " The commit is an "UPSTREAM" commit, as noted above,
  just had conflicts that needed to be resolved. The conflicts are noted
  at the end of the body of the commit message.
* "FROMLIST: " The commit is from an upstream mailing list, and is likely
  to be accepted into upstream, but has not yet landed. The mailing list
  archive URL to the commit, or Message-ID, is noted at the end of the
  body of the commit message.
* "ANDROID: " The commit originates from the Android kernel tree, and
  is not yet upstream.
* "BRILLO: " The commit is Brillo-specific, and is not yet upstream.
* "VENDOR: vendor-name: " The commit comes from a vendor (where
  "vendor-name" is replaced with the vendor), and contains commits not
  yet upstream.
* "RFC: " A temporary state where comments are requested before attempting
  to upstream the commit (after which it would move to "FROMLIST:")

## Commit fields

Each commit should have fields at the end of the commit body:

* "Bug: bug-number" This is required, and references the bug fixed by
  the commit. Used to separate the commit body from the other fields below.
* "(cherry picked from commit SHA...)" This, or a similar line ("... from
  -stable commit SHA...", "... from URL...", "Message-Id: ..."), is required
  to identify where the commit comes from in the case of the "UPSTREAM",
  "BACKPORT", or "FROMLIST" prefixes. For URLs, patchwork.kernel.org links
  are preferred.
* "Signed-off-by: your-name <your-email>" This is required to track the
  origin of changes in the kernel tree.
* "Patchset: name-of-series" This is highly recommended to distinguish
  series of changes that belong together. Using "Bug: " alone for this
  tends not to be sufficient in the case where a feature evolves over time
  or needs additional fixes later.
* "Change-Id: random-id" This is required for Gerrit to to track changes,
  and must be in the last paragraph.

## Example

If the original commit looked like the following, a Brillo common kernel
commit would appear like so (new prefix and fields shown in bold).

### Original commit

> ARM: do awesome stuff
> 
> This fiddles the thing to get impressive results.
> 
> Signed-off-by: Cool Developer <cool@developer.example.com>

### Brillo common kernel commit

> **UPSTREAM:** ARM: do awesome stuff
> 
> This fiddles the thing to get impressive results.
> 
> Signed-off-by: Cool Developer <cool@developer.example.com>
> 
> **Bug: 0123456**  
> **Patchset: be-awesome**  
> 
> **(cherry picked from abc123123123123123123123123)**  
> **Signed-off-by: Kees Cook <keescook@google.com>**  
> **Change-Id: I9999999999999999999999999999999999**  
