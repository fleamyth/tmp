As an RD engineer on Pega’s DIAG team, I would like to clarify the background and timeline of this issue.


We identified the exit-code handling issue on November 19 and immediately reported it to Google. At that time, we had already implemented log-based checks as a temporary workaround to detect the issue and prevent affected DUTs from incorrectly passing the test. This explains the high DUT failure rate observed on November 19.


Google subsequently committed to providing an updated test script to address the issue, which was delivered on November 24.


Therefore, under the same test conditions, we would expect the updated test script to yield a failure rate comparable to that observed on November 19.
